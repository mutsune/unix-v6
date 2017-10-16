#
/*
 */

/*
 *	DH-11 driver
 *	This driver calls on the DHDM driver.
 *	If the DH has no DM11-BB, then the latter will
 *	be fake. To insure loading of the correct DM code,
 *	lib2 should have dhdm.o, dh.o and dhfdm.o in that order.
 */

#include &quot;../param.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;
#include &quot;../tty.h&quot;
#include &quot;../proc.h&quot;

#define	DHADDR	0160020
#define	NDH11	16	/* number of lines */
#define	DHNCH	8	/* max number of DMA chars */

struct	tty dh11[NDH11];
/*
 * Place from which to do DMA on output
 */
char	dh_clist[NDH11][DHNCH];

/*
 * Used to communicate the number of lines to the DM
 */
int	ndh11	NDH11;

/*
 * Hardware control bits
 */
#define	BITS6	01
#define	BITS7	02
#define	BITS8	03
#define	TWOSB	04
#define	PENABLE	020
/* DEC manuals incorrectly say this bit causes generation of even parity. */
#define	OPAR	040
#define	HDUPLX	040000

#define	IENABLE	030100
#define	PERROR	010000
#define	FRERROR	020000
#define	XINT	0100000
#define	SSPEED	7	/* standard speed: 300 baud */

/*
 * Software copy of last dhbar
 */
int	dhsar;

struct dhregs {
	int dhcsr;
	int dhnxch;
	int dhlpr;
	int dhcar;
	int dhbcr;
	int dhbar;
	int dhbreak;
	int dhsilo;
};

/*
 * Open a DH11 line.
 */
dhopen(dev, flag)
{
	register struct tty *tp;
	extern dhstart();

	if (dev.d_minor &gt;= NDH11) {
		u.u_error = ENXIO;
		return;
	}
	tp = &amp;dh11[dev.d_minor];
	tp-&gt;t_addr = dhstart;
	tp-&gt;t_dev = dev;
	DHADDR-&gt;dhcsr =| IENABLE;
	tp-&gt;t_state =| WOPEN|SSTART;
	if ((tp-&gt;t_state&amp;ISOPEN) == 0) {
		tp-&gt;t_erase = CERASE;
		tp-&gt;t_kill = CKILL;
		tp-&gt;t_speeds = SSPEED | (SSPEED&lt;&lt;8);
		tp-&gt;t_flags = ODDP|EVENP|ECHO;
		dhparam(tp);
	}
	dmopen(dev);
	tp-&gt;t_state =&amp; ~WOPEN;
	tp-&gt;t_state =| ISOPEN;
	if (u.u_procp-&gt;p_ttyp == 0)
		u.u_procp-&gt;p_ttyp = tp;
}

/*
 * Close a DH11 line.
 */
dhclose(dev)
{
	register struct tty *tp;

	tp = &amp;dh11[dev.d_minor];
	dmclose(dev);
	tp-&gt;t_state =&amp; (CARR_ON|SSTART);
	wflushtty(tp);
}

/*
 * Read from a DH11 line.
 */
dhread(dev)
{
	ttread(&amp;dh11[dev.d_minor]);
}

/*
 * write on a DH11 line
 */
dhwrite(dev)
{
	ttwrite(&amp;dh11[dev.d_minor]);
}

/*
 * DH11 receiver interrupt.
 */
dhrint()
{
	register struct tty *tp;
	register int c;

	while ((c = DHADDR-&gt;dhnxch) &lt; 0) {	/* char. present */
		tp = &amp;dh11[(c&gt;&gt;8)&amp;017];
		if (tp &gt;= &amp;dh11[NDH11])
			continue;
		if((tp-&gt;t_state&amp;ISOPEN)==0 || (c&amp;PERROR)) {
			wakeup(tp);
			continue;
		}
		if (c&amp;FRERROR)		/* break */
			if (tp-&gt;t_flags&amp;RAW)
				c = 0;		/* null (for getty) */
			else
				c = 0177;	/* DEL (intr) */
		ttyinput(c, tp);
	}
}

/*
 * stty/gtty for DH11
 */
dhsgtty(dev, av)
int *av;
{
	register struct tty *tp;
	register r;

	tp = &amp;dh11[dev.d_minor];
	if (ttystty(tp, av))
		return;
	dhparam(tp);
}

/*
 * Set parameters from open or stty into the DH hardware
 * registers.
 */
dhparam(atp)
struct tty *atp;
{
	register struct tty *tp;
	register int lpr;

	tp = atp;
	spl5();
	DHADDR-&gt;dhcsr.lobyte = tp-&gt;t_dev.d_minor | IENABLE;
	/*
	 * Hang up line?
	 */
	if (tp-&gt;t_speeds.lobyte==0) {
		tp-&gt;t_flags =| HUPCL;
		dmclose(tp-&gt;t_dev);
		return;
	}
	lpr = (tp-&gt;t_speeds.hibyte&lt;&lt;10) | (tp-&gt;t_speeds.lobyte&lt;&lt;6);
	if (tp-&gt;t_speeds.lobyte == 4)		/* 134.5 baud */
		lpr =| BITS6|PENABLE|HDUPLX; else
		if (tp-&gt;t_flags&amp;EVENP)
			if (tp-&gt;t_flags&amp;ODDP)
				lpr =| BITS8; else
				lpr =| BITS7|PENABLE; else
			lpr =| BITS7|OPAR|PENABLE;
	if (tp-&gt;t_speeds.lobyte == 3)	/* 110 baud */
		lpr =| TWOSB;
	DHADDR-&gt;dhlpr = lpr;
	spl0();
}

/*
 * DH11 transmitter interrupt.
 * Restart each line which used to be active but has
 * terminated transmission since the last interrupt.
 */
dhxint()
{
	register struct tty *tp;
	register ttybit, bar;

	bar = dhsar &amp; ~DHADDR-&gt;dhbar;
	DHADDR-&gt;dhcsr =&amp; ~XINT;
	ttybit = 1;
	for (tp = dh11; bar; tp++) {
		if(bar&amp;ttybit) {
			dhsar =&amp; ~ttybit;
			bar =&amp; ~ttybit;
			tp-&gt;t_state =&amp; ~BUSY;
			dhstart(tp);
		}
		ttybit =&lt;&lt; 1;
	}
}

/*
 * Start (restart) transmission on the given DH11 line.
 */
dhstart(atp)
struct tty *atp;
{
	extern ttrstrt();
	register c, nch;
	register struct tty *tp;
	int sps;
	char *cp;

	sps = PS-&gt;integ;
	spl5();
	tp = atp;
	/*
	 * If it&#39;s currently active, or delaying,
	 * no need to do anything.
	 */
	if (tp-&gt;t_state&amp;(TIMEOUT|BUSY))
		goto out;
	/*
	 * t_char is a delay indicator which may have been
	 * left over from the last start.
	 * Arrange for the delay.
	 */
	if (c = tp-&gt;t_char) {
		tp-&gt;t_char = 0;
		timeout(ttrstrt, tp, (c&amp;0177)+6);
		tp-&gt;t_state =| TIMEOUT;
		goto out;
	}
	cp = dh_clist[tp-&gt;t_dev.d_minor];
	nch = 0;
	/*
	 * Copy DHNCH characters, or up to a delay indicator,
	 * to the DMA area.
	 */
	while (nch &gt; -DHNCH &amp;&amp; (c = getc(&amp;tp-&gt;t_outq))&gt;=0) {
		if (c &gt;= 0200) {
			tp-&gt;t_char = c;
			break;
		}
		*cp++ = c;
		nch--;
	}
	/*
	 * If the writer was sleeping on output overflow,
	 * wake him when low tide is reached.
	 */
	if (tp-&gt;t_outq.c_cc&lt;=TTLOWAT &amp;&amp; tp-&gt;t_state&amp;ASLEEP) {
		tp-&gt;t_state =&amp; ~ASLEEP;
		wakeup(&amp;tp-&gt;t_outq);
	}
	/*
	 * If any characters were set up, start transmission;
	 * otherwise, check for possible delay.
	 */
	if (nch) {
		DHADDR-&gt;dhcsr.lobyte = tp-&gt;t_dev.d_minor | IENABLE;
		DHADDR-&gt;dhcar = cp+nch;
		DHADDR-&gt;dhbcr = nch;
		c = 1&lt;&lt;tp-&gt;t_dev.d_minor;
		DHADDR-&gt;dhbar =| c;
		dhsar =| c;
		tp-&gt;t_state =| BUSY;
	} else if (c = tp-&gt;t_char) {
		tp-&gt;t_char = 0;
		timeout(ttrstrt, tp, (c&amp;0177)+6);
		tp-&gt;t_state =| TIMEOUT;
	}
    out:
	PS-&gt;integ = sps;
}
