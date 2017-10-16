#
/*
 */

/*
 * RP04 disk driver
 *
 * This driver has been tested on a working RP04 for a few hours.
 * It does not attempt ECC error correction and is probably
 * deficient in general in the case of errors and when packs
 * are dismounted.
 */

#include &quot;../param.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;

struct {
	int	hpcs1;	/* Control and Status register 1 */
	int	hpwc;	/* Word count register */
	int	hpba;	/* UNIBUS address register */
	int	hpda;	/* Desired address register */
	int	hpcs2;	/* Control and Status register 2*/
	int	hpds;	/* Drive Status */
	int	hper1;	/* Error register 1 */
	int	hpas;	/* Attention Summary */
	int	hpla;	/* Look ahead */
	int	hpdb;	/* Data buffer */
	int	hpmr;	/* Maintenance register */
	int	hpdt;	/* Drive type */
	int	hpsn;	/* Serial number */
	int	hpof;	/* Offset register */
	int	hpca;	/* Desired Cylinder address register*/
	int	hpcc;	/* Current Cylinder */
	int	hper2;	/* Error register 2 */
	int	hper3;	/* Error register 3 */
	int	hppos;	/* Burst error bit position */
	int	hppat;	/* Burst error bit pattern */
	int	hpbae;	/* 11/70 bus extension */
};

#define	HPADDR	0176700
#define	NHP	8

struct {
	char	*nblocks;
	int	cyloff;
} hp_sizes[] {
	9614,	0,		/* cyl 0 thru 23 */
				/* cyl 24 thru 43 available */
	-1,	44,		/* cyl 44 thru 200 */
	-1,	201,		/* cyl 201 thru 357 */
	20900,	358,		/* cyl 358 thru 407 */
				/* cyl 408 thru 410 blank */
	40600,	0,
	40600,	100,
	40600,	200,
	40600,	300,
};


struct	devtab	hptab;
struct	buf	hpbuf;

char	hp_openf;

			/* Drive Commands */
#define	GO	01
#define	PRESET	020
#define	RECAL	06
#define RCLR	010
#define OFFSET	014

#define	READY	0200	/* hpds - drive ready */
#define	PIP	020000	/* hpds - Positioning Operation in Progress */
#define	ERR	040000	/* hpcs1 - composite error */

#define	DU	040000	/* hper1 - Drive Unsafe	*/
#define	DTE	010000  /* hper1 - Drive Timing Error	*/
#define	OPI	020000  /* hper1 - Operation Incomplete	*/
		/* Error Correction Code errors */
#define DCK	0100000	/* hper1 - Data Check error */
#define ECH	0100    /* hper1 - ECC hard error */

#define CLR	040	/* hpcs2 - Controller Clear */

#define FMT22	010000	/* hpof - 16 bit /word format */
/*
 * Use av_back to save track+sector,
 * b_resid for cylinder.
 */

#define	trksec	av_back
#define	cylin	b_resid

hpopen()
{

	if(!hp_openf){
		hp_openf++;
		HPADDR-&gt;hpcs2 = CLR;
		HPADDR-&gt;hpcs1 = RCLR|GO;
		HPADDR-&gt;hpcs1 = PRESET|GO;
		HPADDR-&gt;hpof = FMT22;
	}
}

hpstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;
	register char *p1, *p2;

	bp = abp;
	p1 = &amp;hp_sizes[bp-&gt;b_dev.d_minor&amp;07];
	if (bp-&gt;b_dev.d_minor &gt;= (NHP&lt;&lt;3) ||
	    bp-&gt;b_blkno &gt;= p1-&gt;nblocks) {
		bp-&gt;b_flags =| B_ERROR;
		iodone(bp);
		return;
	}
	bp-&gt;av_forw = 0;
	bp-&gt;cylin = p1-&gt;cyloff;
	p1 = bp-&gt;b_blkno;
	p2 = lrem(p1, 22);
	p1 = ldiv(p1, 22);
	bp-&gt;trksec = (p1%19)&lt;&lt;8 | p2;
	bp-&gt;cylin =+ p1/19;
	spl5();
	if ((p1 = hptab.d_actf)==0)
		hptab.d_actf = bp;
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
	if (hptab.d_active==0)
		hpstart();
	spl0();
}

hpstart()
{
	register struct buf *bp;

	if ((bp = hptab.d_actf) == 0)
		return;
	hptab.d_active++;
	HPADDR-&gt;hpcs2 = bp-&gt;b_dev.d_minor &gt;&gt; 3;
	HPADDR-&gt;hpca = bp-&gt;cylin;
	rhstart(bp, &amp;HPADDR-&gt;hpda, bp-&gt;trksec, &amp;HPADDR-&gt;hpbae);
}

hpintr()
{
	register struct buf *bp;
	register int ctr;

	if (hptab.d_active == 0)
		return;
	bp = hptab.d_actf;
	hptab.d_active = 0;
	if (HPADDR-&gt;hpcs1 &amp; ERR) {		/* error bit */
		deverror(bp, HPADDR-&gt;hpcs2, 0);
		if(HPADDR-&gt;hper1 &amp; (DU|DTE|OPI)) {
			HPADDR-&gt;hpcs2 = CLR;
			HPADDR-&gt;hpcs1 = RECAL|GO;
			ctr = 0;
			while ((HPADDR-&gt;hpds&amp;PIP) &amp;&amp; --ctr);
		}
		HPADDR-&gt;hpcs1 = RCLR|GO;
		if (++hptab.d_errcnt &lt;= 10) {
			hpstart();
			return;
		}
		bp-&gt;b_flags =| B_ERROR;
	}
	hptab.d_errcnt = 0;
	hptab.d_actf = bp-&gt;av_forw;
	bp-&gt;b_resid = HPADDR-&gt;hpwc;
	iodone(bp);
	hpstart();
}

hpread(dev)
{

	if(hpphys(dev))
	physio(hpstrategy, &amp;hpbuf, dev, B_READ);
}

hpwrite(dev)
{

	if(hpphys(dev))
	physio(hpstrategy, &amp;hpbuf, dev, B_WRITE);
}

hpphys(dev)
{
	register c;

	c = lshift(u.u_offset, -9);
	c =+ ldiv(u.u_count+511, 512);
	if(c &gt; hp_sizes[dev.d_minor &amp; 07].nblocks) {
		u.u_error = ENXIO;
		return(0);
	}
	return(1);
}

