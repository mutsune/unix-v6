#
/*
 */

/*
 * DP-11 Synchronous interface driver
 * This driver is rather insensitive to the remote
 * device it talks to, which is to say most of the protocol
 * must be supplied by the calling program.
 * Exceptions: parity is even; 7 data bits per character;
 * max. of 512 characters per record; 10 second timeout
 * on waiting to receive; half-duplex transmission.
 */

#include &quot;../param.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;
#include &quot;../buf.h&quot;

/* control info */
struct {
	char	*dp_buf;
	char	*dp_bufp;
	int	dp_nxmit;
	char	dp_state;
	char	dp_timer;
	int	dp_proc;
} dp11;

/* device registers */
struct {
	int	dprcsr;
	char	dprbuf;
	char	dpsyn0;
	int	dptcsr;
	char	dptbuf;
	char	dpsyn1;
};

/* bits */
#define	ODDPAR	010000
#define	IENABLE	0100
#define	HDUPLX	02

#define	CTRANS	0100000
#define	RORUN	040000
#define	RING	020000
#define	DSRDY	010000
#define	CARRIER	04000
#define	DONE	0200
#define	IENABLE	0100
#define	SIENABL	040

#define	WRITE	1
#define	READ	0

#define	DTRDY	01
#define	RCVACT	04000

#define	DPADDR	0174770
#define	DPPRI	5
#define	SYN	026		/* (receive) sync character */

/*
 * The open fails unless the device is not open or
 * the opening process is the one that has it open already.
 */
dpopen(dev, flag)
{
	int dptimeout();

	if (dp11.dp_proc!=0 &amp;&amp; dp11.dp_proc!=u.u_procp) {
		u.u_error = ENXIO;
		return;
	}
	dp11.dp_proc = u.u_procp;
	dp11.dp_state = READ;
	if (dp11.dp_buf==0) {
		dp11.dp_buf = getblk(NODEV);
		dp11.dp_bufp = dp11.dp_buf-&gt;b_addr;
		dp11.dp_timer = HZ;
		timeout(dptimeout, 0, HZ);
	}
	DPADDR-&gt;dpsyn0 = SYN;
	DPADDR-&gt;dprcsr = HDUPLX|IENABLE;
	DPADDR-&gt;dptcsr = IENABLE|SIENABL|DTRDY;
}

dpclose()
{
	DPADDR-&gt;dprcsr = 0;
	DPADDR-&gt;dptcsr = 0;
	dp11.dp_timer = 0;
	dp11.dp_proc = 0;
	if (dp11.dp_buf != 0) {
		brelse(dp11.dp_buf);
		dp11.dp_buf = 0;
	}
}

/*
 * Read waits until:
 *  there is loss of &quot;data set ready&quot;, or
 *  a timeout occurs, or
 *  a full record has been received.
 * The former two result in an error.
 */
dpread()
{
	register char *bp, **epp;

	bp = dp11.dp_buf-&gt;b_addr;
	epp = &amp;dp11.dp_bufp;
	for(;;) {
		if(dpwait())
			return;
		if (*epp &gt; bp)
			break;
		spl6();
		if (dp11.dp_timer &lt;= 1) {
			spl0();
			return;
		}
		sleep(&amp;dp11, DPPRI);
		spl0();
	}
	iomove(dp11.dp_buf, 0, min(u.u_count, *epp-bp), B_READ);
}

/*
 * write checks to make sure that the data set is not reading,
 * and that it is ready.  Then the record is copied
 * and transmission started.
 */
dpwrite()
{
	register char *bp;

	if (u.u_count==0 || dpwait())
		return;
	dp11.dp_state = WRITE;
	bp = dp11.dp_buf-&gt;b_addr;
	dp11.dp_bufp = bp;
	if (u.u_count&gt;512)
		u.u_count = 512;
	dp11.dp_nxmit = u.u_count;
	iomove(dp11.dp_buf, 0, u.u_count, B_WRITE);
	dpstart();
}

/*
 * If &quot;data set ready&quot; is down return an error; otherwise
 * wait until the dataset is in read state with no carrier,
 * which means a record has just been received.
 */
dpwait()
{
	for(;;) {
		if ((DPADDR-&gt;dptcsr&amp;DSRDY)==0 || dp11.dp_buf==0) {
			u.u_error = EIO;
			return(1);
		}
		spl6();
		if (dp11.dp_state==READ &amp;&amp; (DPADDR-&gt;dptcsr&amp;CARRIER)==0) {
			spl0();
			return(0);
		}
		sleep(&amp;dp11, DPPRI);
		spl0();
	}
}

/*
 * Start off the next character to be transmitted;
 * when the record is done, drop back into read state.
 */
dpstart()
{
	register int c;
	extern char partab[];

	dp11.dp_timer = 10;
	if (--dp11.dp_nxmit &gt;= 0) {
		c = (*dp11.dp_bufp++) &amp; 0177;
		DPADDR-&gt;dptbuf = c | ~partab[c]&amp;0200;
	} else {
		dp11.dp_bufp = dp11.dp_buf-&gt;b_addr;
		dp11.dp_state = READ;
	}
}

/*
 * Count down the DP timer (once per second)
 * If it runs out, it presumably means the other station
 * won&#39;t speak.
 */
dptimeout()
{
	if (dp11.dp_timer==0)
		return;
	if (--dp11.dp_timer==0) {
		dpturnaround();
		dp11.dp_timer = 1;
	}
	timeout(dptimeout, 0, HZ);
}

/*
 * Receiver interrupt: if reading, stash character
 * unless there is an overrun.
 */
dprint()
{
	register int c;

	c = DPADDR-&gt;dprbuf &amp; 0177;
	if (dp11.dp_state==READ) {
		if ((DPADDR-&gt;dprcsr&amp;ODDPAR) == 0)
			c =| 0200;
		if (dp11.dp_bufp &lt; dp11.dp_buf-&gt;b_addr+512)
			*dp11.dp_bufp++ = c;
	}
}

/*
 * Transmitter interrupt:
 * Knock down hardware bits.
 * If carrier has dropped, the record is done, so turn the line around;
 * otherwise start another character.
 */
dpxint()
{
	register int dpstat;

	dpstat = DPADDR-&gt;dptcsr;
	DPADDR-&gt;dptcsr =&amp; ~(CTRANS|RORUN|RING|DONE);
	if (dpstat &amp; (CTRANS|RORUN))
		dpturnaround();
	else if (dpstat&amp;DONE &amp;&amp; dp11.dp_state==WRITE)
		dpstart();
}

/*
 * Change the state from writing to reading at the end of a record.
 */
dpturnaround()
{
	DPADDR-&gt;dprcsr =&amp; ~RCVACT;
	if (dp11.dp_state==WRITE) {
		dp11.dp_timer = 10;
		dp11.dp_state = READ;
		dp11.dp_bufp = dp11.dp_buf-&gt;b_addr;
	}
	wakeup(&amp;dp11);
}
