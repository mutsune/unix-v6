#
/*
 */

/*
 * RS03/04 disk driver
 */

#include &quot;../param.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;


struct {
	int	hscs1;	/* Control and Status register 1 */
	int	hswc;	/* Word count register */
	int	hsba;	/* UNIBUS address register */
	int	hsda;	/* Desired address register */
	int	hscs2;	/* Control and Status register 2 */
	int	hsds;	/* Drive Status */
	int	hser;	/* Error register */
	int	hsas;	/* not used */
	int	hsla;	/* not used */
	int	hsdb;	/* not used */
	int	hsmr;	/* not used */
	int	hsdt;	/* not used */
	int	hsbae;	/* 11/70 bus extension */
};

struct	devtab	hstab;
struct	buf	rhsbuf;

#define	HSADDR	0172040

#define ERR	040000	/* hscs1 - composite error */

#define GO	01
#define RCLR	010
#define	DRY	0200	/* hsds - Drive Ready */

hsstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;
	register mblks;

	bp = abp;
	mblks = 1024; /* RJS03 */
	if(bp-&gt;b_dev.d_minor &gt;= 8)
		mblks = 2048; /* RJS04 */
	if(bp-&gt;b_blkno &gt;= mblks) {
		bp-&gt;b_flags =| B_ERROR;
		iodone(bp);
		return;
	}
	bp-&gt;av_forw = 0;
	spl5();
	if (hstab.d_actf==0)
		hstab.d_actf = bp; else
		hstab.d_actl-&gt;av_forw = bp;
	hstab.d_actl = bp;
	if (hstab.d_active==0)
		hsstart();
	spl0();
}

hsstart()
{
	register struct buf *bp;
	register addr;

	if ((bp = hstab.d_actf) == 0)
		return;
	hstab.d_active++;
	addr = bp-&gt;b_blkno;
	if(bp-&gt;b_dev.d_minor &lt; 8)
		addr =&lt;&lt; 1; /* RJS03 */
	HSADDR-&gt;hscs2 = bp-&gt;b_dev.d_minor &amp; 07;
	rhstart(bp, &amp;HSADDR-&gt;hsda, addr&lt;&lt;1, &amp;HSADDR-&gt;hsbae);
}

hsintr()
{
	register struct buf *bp;

	if (hstab.d_active == 0)
		return;
	bp = hstab.d_actf;
	hstab.d_active = 0;
	if(HSADDR-&gt;hscs1 &amp; ERR){	/* error bit */
		deverror(bp, HSADDR-&gt;hscs2, 0);
		HSADDR-&gt;hscs1 = RCLR|GO;
		if (++hstab.d_errcnt &lt;= 10) {
			hsstart();
			return;
		}
		bp-&gt;b_flags =| B_ERROR;
	}
	hstab.d_errcnt = 0;
	hstab.d_actf = bp-&gt;av_forw;
	iodone(bp);
	hsstart();
}

hsread(dev)
{

	physio(hsstrategy, &amp;rhsbuf, dev, B_READ);
}

hswrite(dev)
{

	physio(hsstrategy, &amp;rhsbuf, dev, B_WRITE);
}
