#
/*
 */

/*
 * RF disk driver
 */

#include &quot;../param.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;

struct {
	int	rfcs;
	int	rfwc;
	int	rfba;
	int	rfda;
	int	rfdae;
};

struct	devtab	rftab;
struct	buf	rrfbuf;

#define	NRFBLK	1024
#define	RFADDR	0177460

#define	GO	01
#define	RCOM	02
#define	WCOM	04
#define	CTLCLR	0400
#define	IENABLE	0100

rfstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;

	bp = abp;
	if(bp-&gt;b_flags&amp;B_PHYS)
		mapalloc(bp);
	if (bp-&gt;b_blkno &gt;= NRFBLK*(bp-&gt;b_dev.d_minor+1)) {
		bp-&gt;b_flags =| B_ERROR;
		iodone(bp);
		return;
	}
	bp-&gt;av_forw = 0;
	spl5();
	if (rftab.d_actf==0)
		rftab.d_actf = bp;
	else
		rftab.d_actl-&gt;av_forw = bp;
	rftab.d_actl = bp;
	if (rftab.d_active==0)
		rfstart();
	spl0();
}

rfstart()
{
	register struct buf *bp;

	if ((bp = rftab.d_actf) == 0)
		return;
	rftab.d_active++;
	RFADDR-&gt;rfdae = bp-&gt;b_blkno.hibyte;
	devstart(bp, &amp;RFADDR-&gt;rfda, bp-&gt;b_blkno&lt;&lt;8, 0);
}

rfintr()
{
	register struct buf *bp;

	if (rftab.d_active == 0)
		return;
	bp = rftab.d_actf;
	rftab.d_active = 0;
	if (RFADDR-&gt;rfcs &lt; 0) {		/* error bit */
		deverror(bp, RFADDR-&gt;rfcs, RFADDR-&gt;rfdae);
		RFADDR-&gt;rfcs = CTLCLR;
		if (++rftab.d_errcnt &lt;= 10) {
			rfstart();
			return;
		}
		bp-&gt;b_flags =| B_ERROR;
	}
	rftab.d_errcnt = 0;
	rftab.d_actf = bp-&gt;av_forw;
	iodone(bp);
	rfstart();
}

rfread(dev)
{

	physio(rfstrategy, &amp;rrfbuf, dev, B_READ);
}

rfwrite(dev)
{

	physio(rfstrategy, &amp;rrfbuf, dev, B_WRITE);
}
