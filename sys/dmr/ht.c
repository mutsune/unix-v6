#
/*
 */

/*
 * TJU16 tape driver
 */

#include &quot;../param.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;

struct {
	int	htcs1;
	int	htwc;
	int	htba;
	int	htfc;
	int	htcs2;
	int	htds;
	int	hter;
	int	htas;
	int	htck;
	int	htdb;
	int	htmr;
	int	htdt;
	int	htsn;
	int	httc;
	int	htbae;	/* 11/70 bus extension */
};

struct	devtab	httab;
struct	buf	rhtbuf;

#define	NUNIT	8

char	h_openf[NUNIT];
char	*h_blkno[NUNIT];
char	*h_nxrec[NUNIT];

#define	HTADDR	0172440

#define	GO	01
#define	NOP	0
#define	WEOF	026
#define	SFORW	030
#define	SREV	032
#define	ERASE	024
#define	REW	06
#define	CLR	010
#define	P800	01300		/* 800 + pdp11 mode */
#define	P1600	02300		/* 1600 + pdp11 mode */
#define	IENABLE	0100
#define	CRDY	0200
#define	EOF	04
#define	DRY	0200
#define	MOL	010000
#define	PIP	020000
#define	ERR	040000

#define	SSEEK	1
#define	SIO	2

htopen(dev, flag)
{
	register unit;

	unit = dev.d_minor&amp;077;
	if (unit &gt;= NUNIT || h_openf[unit])
		u.u_error = ENXIO;
	else {
		h_openf[unit]++;
		h_blkno[unit] = 0;
		h_nxrec[unit] = 65535;
		hcommand(dev, NOP);
	}
}

htclose(dev, flag)
{
	register int unit;

	unit = dev.d_minor&amp;077;
	h_openf[unit] = 0;
	if (flag) {
		hcommand(dev, WEOF);
		hcommand(dev, WEOF);
	}
	hcommand(dev, REW);
}

hcommand(dev, com)
{
	register unit;
	extern lbolt;

	unit = dev.d_minor;
	while (httab.d_active || (HTADDR-&gt;htcs1 &amp; CRDY)==0)
		sleep(&amp;lbolt, 1);
	HTADDR-&gt;htcs2 = (unit&gt;&gt;3)&amp;07;
	while((HTADDR-&gt;htds&amp;DRY) == 0)
		sleep(&amp;lbolt, 1);
	if(unit &gt;= 64)
		HTADDR-&gt;httc = P800 | (unit&amp;07); else
		HTADDR-&gt;httc = P1600 | (unit&amp;07);
	while((HTADDR-&gt;htds&amp;(MOL|PIP)) != MOL)
		sleep(&amp;lbolt, 1);
	HTADDR-&gt;htcs1 = com | GO;
}

htstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;
	register char **p;

	bp = abp;
	p = &amp;h_nxrec[bp-&gt;b_dev.d_minor&amp;077];
	if (*p &lt;= bp-&gt;b_blkno) {
		if (*p &lt; bp-&gt;b_blkno) {
			bp-&gt;b_flags =| B_ERROR;
			iodone(bp);
			return;
		}
		if (bp-&gt;b_flags&amp;B_READ) {
			clrbuf(bp);
			iodone(bp);
			return;
		}
	}
	if ((bp-&gt;b_flags&amp;B_READ)==0)
		*p = bp-&gt;b_blkno + 1;
	bp-&gt;av_forw = 0;
	spl5();
	if (httab.d_actf==0)
		httab.d_actf = bp;
	else
		httab.d_actl-&gt;av_forw = bp;
	httab.d_actl = bp;
	if (httab.d_active==0)
		htstart();
	spl0();
}

htstart()
{
	register struct buf *bp;
	register int unit;
	register char *blkno;

    loop:
	if ((bp = httab.d_actf) == 0)
		return;
	unit = bp-&gt;b_dev.d_minor;
	HTADDR-&gt;htcs2 = (unit&gt;&gt;3)&amp;07;
	if(unit &gt;= 64)
		HTADDR-&gt;httc = P800 | (unit&amp;07); else
		HTADDR-&gt;httc = P1600 | (unit&amp;07);
	unit =&amp; 077;
	blkno = h_blkno[unit];
	if (h_openf[unit] &lt; 0 || (HTADDR-&gt;htcs1 &amp; CRDY)==0) {
		bp-&gt;b_flags =| B_ERROR;
		httab.d_actf = bp-&gt;av_forw;
		iodone(bp);
		goto loop;
	}
	if (blkno != bp-&gt;b_blkno) {
		httab.d_active = SSEEK;
		if (blkno &lt; bp-&gt;b_blkno) {
			HTADDR-&gt;htfc = blkno - bp-&gt;b_blkno;
			HTADDR-&gt;htcs1 = SFORW|IENABLE|GO;
		} else {
			if (bp-&gt;b_blkno == 0)
				HTADDR-&gt;htcs1 = REW|IENABLE|GO;
			else {
				HTADDR-&gt;htfc = bp-&gt;b_blkno - blkno;
				HTADDR-&gt;htcs1 = SREV|IENABLE|GO;
			}
		}
		return;
	}
	httab.d_active = SIO;
	rhstart(bp, &amp;HTADDR-&gt;htfc, bp-&gt;b_wcount&lt;&lt;1, &amp;HTADDR-&gt;htbae);
}

htintr()
{
	register struct buf *bp;
	register int unit;

	if ((bp = httab.d_actf)==0)
		return;
	unit = bp-&gt;b_dev.d_minor&amp;077;
	if (HTADDR-&gt;htcs1 &amp; ERR) {
/*
		deverror(bp, HTADDR-&gt;hter, 0);
 */
		if(HTADDR-&gt;htds&amp;EOF) {
			if(bp != &amp;rhtbuf &amp;&amp; h_openf[unit])
				h_openf[unit] = -1;
		}
		HTADDR-&gt;htcs1 = ERR|CLR|GO;
		if ((HTADDR-&gt;htds&amp;DRY)!=0 &amp;&amp; httab.d_active==SIO) {
			if (++httab.d_errcnt &lt; 10) {
				h_blkno[unit]++;
				httab.d_active = 0;
				htstart();
				return;
			}
		}
		bp-&gt;b_flags =| B_ERROR;
		httab.d_active = SIO;
	}
	if (httab.d_active == SIO) {
		httab.d_errcnt = 0;
		h_blkno[unit]++;
		httab.d_actf = bp-&gt;av_forw;
		httab.d_active = 0;
		iodone(bp);
		bp-&gt;b_resid = HTADDR-&gt;htfc;
	} else
		h_blkno[unit] = bp-&gt;b_blkno;
	htstart();
}

htread(dev)
{
	htphys(dev);
	physio(htstrategy, &amp;rhtbuf, dev, B_READ);
	u.u_count = -rhtbuf.b_resid;
}

htwrite(dev)
{
	htphys(dev);
	physio(htstrategy, &amp;rhtbuf, dev, B_WRITE);
	u.u_count = 0;
}

htphys(dev)
{
	register unit, a;

	unit = dev.d_minor;
	a = lshift(u.u_offset, -9);
	h_blkno[unit] = a;
	h_nxrec[unit] = ++a;
}
