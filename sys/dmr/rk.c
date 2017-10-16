#
/*
 */

/*
 * RK disk driver
 */

#include &quot;../param.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;

#define	RKADDR	0177400
#define	NRK	4
#define	NRKBLK	4872

#define	RESET	0
#define	GO	01
#define	DRESET	014
#define	IENABLE	0100
#define	DRY	0200
#define	ARDY	0100
#define	WLO	020000
#define	CTLRDY	0200

struct {
	int rkds;
	int rker;
	int rkcs;
	int rkwc;
	int rkba;
	int rkda;
};

struct	devtab	rktab;
struct	buf	rrkbuf;

rkstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;
	register *qc, *ql;
	int d;

	bp = abp;
	if(bp-&gt;b_flags&amp;B_PHYS)
		mapalloc(bp);
	d = bp-&gt;b_dev.d_minor-7;
	if(d &lt;= 0)
		d = 1;
	if (bp-&gt;b_blkno &gt;= NRKBLK*d) {
		bp-&gt;b_flags =| B_ERROR;
		iodone(bp);
		return;
	}
	bp-&gt;av_forw = 0;
	spl5();
	if (rktab.d_actf==0)
		rktab.d_actf = bp;
	else
		rktab.d_actl-&gt;av_forw = bp;
	rktab.d_actl = bp;
	if (rktab.d_active==0)
		rkstart();
	spl0();
}

rkaddr(bp)
struct buf *bp;
{
	register struct buf *p;
	register int b;
	int d, m;

	p = bp;
	b = p-&gt;b_blkno;
	m = p-&gt;b_dev.d_minor - 7;
	if(m &lt;= 0)
		d = p-&gt;b_dev.d_minor;
	else {
		d = lrem(b, m);
		b = ldiv(b, m);
	}
	return(d&lt;&lt;13 | (b/12)&lt;&lt;4 | b%12);
}

rkstart()
{
	register struct buf *bp;

	if ((bp = rktab.d_actf) == 0)
		return;
	rktab.d_active++;
	devstart(bp, &amp;RKADDR-&gt;rkda, rkaddr(bp), 0);
}

rkintr()
{
	register struct buf *bp;

	if (rktab.d_active == 0)
		return;
	bp = rktab.d_actf;
	rktab.d_active = 0;
	if (RKADDR-&gt;rkcs &lt; 0) {		/* error bit */
		deverror(bp, RKADDR-&gt;rker, RKADDR-&gt;rkds);
		RKADDR-&gt;rkcs = RESET|GO;
		while((RKADDR-&gt;rkcs&amp;CTLRDY) == 0) ;
		if (++rktab.d_errcnt &lt;= 10) {
			rkstart();
			return;
		}
		bp-&gt;b_flags =| B_ERROR;
	}
	rktab.d_errcnt = 0;
	rktab.d_actf = bp-&gt;av_forw;
	iodone(bp);
	rkstart();
}

rkread(dev)
{

	physio(rkstrategy, &amp;rrkbuf, dev, B_READ);
}

rkwrite(dev)
{

	physio(rkstrategy, &amp;rrkbuf, dev, B_WRITE);
}
