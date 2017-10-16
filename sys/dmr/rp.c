#
/*
 */

/*
 * RP disk driver
 */

#include &quot;../param.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;

struct {
	int	rpds;
	int	rper;
	int	rpcs;
	int	rpwc;
	int	rpba;
	int	rpca;
	int	rpda;
};

#define	RPADDR	0176710
#define	NRP	8

struct {
	char	*nblocks;
	int	cyloff;
} rp_sizes[] {
	40600,	0,		/* cyl 0 thru 202 */
	40600,	203,		/* cyl 203 thru 405 */
	9200,	0,		/* cyl 0 thru 45 */
	9200,	360,		/* cyl 360 thru 405 */
	-1,	0,		/* cyl 0 thru 327 */
	-1,	78,		/* cyl 78 thru 405 */
	15600,	0,		/* cyl 0 thru 77 */
	15600,	328,		/* cyl 328 thru 405 */
};

struct	devtab	rptab;
struct	buf	rrpbuf;

#define	GO	01
#define	RESET	0
#define	HSEEK	014

#define	IENABLE	0100
#define	READY	0200

#define	SUFU	01000
#define	SUSU	02000
#define	SUSI	04000
#define	HNF	010000

/*
 * Use av_back to save track+sector,
 * b_resid for cylinder.
 */

#define	trksec	av_back
#define	cylin	b_resid

rpstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;
	register char *p1, *p2;

	bp = abp;
	if(bp-&gt;b_flags&amp;B_PHYS)
		mapalloc(bp);
	p1 = &amp;rp_sizes[bp-&gt;b_dev.d_minor&amp;07];
	if (bp-&gt;b_dev.d_minor &gt;= (NRP&lt;&lt;3) ||
	    bp-&gt;b_blkno &gt;= p1-&gt;nblocks) {
		bp-&gt;b_flags =| B_ERROR;
		iodone(bp);
		return;
	}
	bp-&gt;av_forw = 0;
	bp-&gt;cylin = p1-&gt;cyloff;
	p1 = bp-&gt;b_blkno;
	p2 = lrem(p1, 10);
	p1 = ldiv(p1, 10);
	bp-&gt;trksec = (p1%20)&lt;&lt;8 | p2;
	bp-&gt;cylin =+ p1/20;
	spl5();
	if ((p1 = rptab.d_actf)==0)
		rptab.d_actf = bp;
	else {
		for (; p2 = p1-&gt;av_forw; p1 = p2) {
			if (p1-&gt;cylin &lt;= bp-&gt;cylin
			 &amp;&amp; bp-&gt;cylin &lt;  p2-&gt;cylin
			 || p1-&gt;cylin &gt;= bp-&gt;cylin
			 &amp;&amp; bp-&gt;cylin &gt;  p2-&gt;cylin) 
				break;
		}
		bp-&gt;av_forw = p2;
		p1-&gt;av_forw = bp;
	}
	if (rptab.d_active==0)
		rpstart();
	spl0();
}

rpstart()
{
	register struct buf *bp;

	if ((bp = rptab.d_actf) == 0)
		return;
	rptab.d_active++;
	RPADDR-&gt;rpda = bp-&gt;trksec;
	devstart(bp, &amp;RPADDR-&gt;rpca, bp-&gt;cylin, bp-&gt;b_dev.d_minor&gt;&gt;3);
}

rpintr()
{
	register struct buf *bp;
	register int ctr;

	if (rptab.d_active == 0)
		return;
	bp = rptab.d_actf;
	rptab.d_active = 0;
	if (RPADDR-&gt;rpcs &lt; 0) {		/* error bit */
		deverror(bp, RPADDR-&gt;rper, RPADDR-&gt;rpds);
		if(RPADDR-&gt;rpds &amp; (SUFU|SUSI|HNF)) {
			RPADDR-&gt;rpcs.lobyte = HSEEK|GO;
			ctr = 0;
			while ((RPADDR-&gt;rpds&amp;SUSU) &amp;&amp; --ctr);
		}
		RPADDR-&gt;rpcs = RESET|GO;
		ctr = 0;
		while ((RPADDR-&gt;rpcs&amp;READY) == 0 &amp;&amp; --ctr);
		if (++rptab.d_errcnt &lt;= 10) {
			rpstart();
			return;
		}
		bp-&gt;b_flags =| B_ERROR;
	}
	rptab.d_errcnt = 0;
	rptab.d_actf = bp-&gt;av_forw;
	bp-&gt;b_resid = RPADDR-&gt;rpwc;
	iodone(bp);
	rpstart();
}

rpread(dev)
{

	if(rpphys(dev))
	physio(rpstrategy, &amp;rrpbuf, dev, B_READ);
}

rpwrite(dev)
{

	if(rpphys(dev))
	physio(rpstrategy, &amp;rrpbuf, dev, B_WRITE);
}

rpphys(dev)
{
	register c;

	c = lshift(u.u_offset, -9);
	c =+ ldiv(u.u_count+511, 512);
	if(c &gt; rp_sizes[dev.d_minor &amp; 07].nblocks) {
		u.u_error = ENXIO;
		return(0);
	}
	return(1);
}
