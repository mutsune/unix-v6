#
/*
 */

/*
 * TC-11 DECtape driver
 */

#include &quot;../param.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../user.h&quot;

struct {
	int	tccsr;
	int	tccm;
	int	tcwc;
	int	tcba;
	int	tcdt;
};

struct	devtab	tctab;
char	tcper[8];

#define	TCADDR	0177340
#define	NTCBLK	578

#define	TAPERR	0100000
#define	TREV	04000
#define	READY	0200
#define	IENABLE	0100
#define	UPS	0200
#define	ENDZ	0100000
#define	BLKM	02000
#define	ILGOP	010000
#define	SELERR	04000

#define	SAT	0
#define	RNUM	02
#define	RDATA	04
#define	SST	010
#define	WDATA	014
#define	GO	01

#define	SFORW	1
#define	SREV	2
#define	SIO	3

tcclose(dev)
{
	bflush(dev);
	tcper[dev&amp;07] = 0;
}

tcstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;

	bp = abp;
	if(bp-&gt;b_flags&amp;B_PHYS)
		mapalloc(bp);
	if(bp-&gt;b_blkno &gt;= NTCBLK || tcper[bp-&gt;b_dev&amp;07]) {
		bp-&gt;b_flags =| B_ERROR;
		iodone(bp);
		return;
	}
	bp-&gt;av_forw = 0;
	spl6();
	if (tctab.d_actf==0)
		tctab.d_actf = bp;
	else
		tctab.d_actl-&gt;av_forw = bp;
	tctab.d_actl = bp;
	if (tctab.d_active==0)
		tcstart();
	spl0();
}

tcstart()
{
	register struct buf *bp;
	register int *tccmp, com;

loop:
	tccmp = &amp;TCADDR-&gt;tccm;
	if ((bp = tctab.d_actf) == 0)
		return;
	if(tcper[bp-&gt;b_dev&amp;07]) {
		if((tctab.d_actf = bp-&gt;av_forw) == 0)
			(*tccmp).lobyte = SAT|GO;
		bp-&gt;b_flags =| B_ERROR;
		iodone(bp);
		goto loop;
	}
	if (((*tccmp).hibyte&amp;07) != bp-&gt;b_dev.d_minor)
		(*tccmp).lobyte = SAT|GO;
	tctab.d_errcnt = 20;
	tctab.d_active = SFORW;
	com = (bp-&gt;b_dev.d_minor&lt;&lt;8) | IENABLE|RNUM|GO;
	if ((TCADDR-&gt;tccsr &amp; UPS) == 0) {
		com =| TREV;
		tctab.d_active = SREV;
	}
	*tccmp = com;
}

tcintr()
{
	register struct buf *bp;
	register int *tccmp;
	register int *tcdtp;

	tccmp = &amp;TCADDR-&gt;tccm;
	tcdtp = &amp;TCADDR-&gt;tccsr;
	bp = tctab.d_actf;
	if (*tccmp&amp;TAPERR) {
		if((*tcdtp&amp;(ENDZ|BLKM)) == 0)
			deverror(bp, *tcdtp, 0);
		if(*tcdtp &amp; (ILGOP|SELERR)) {
			tcper[bp-&gt;b_dev&amp;07]++;
			tctab.d_errcnt = 0;
		}
		*tccmp =&amp; ~TAPERR;
		if (--tctab.d_errcnt  &lt;= 0) {
			bp-&gt;b_flags =| B_ERROR;
			goto done;
		}
		if (*tccmp&amp;TREV) {
		setforw:
			tctab.d_active = SFORW;
			*tccmp =&amp; ~TREV;
		} else {
		setback:
			tctab.d_active = SREV;
			*tccmp =| TREV;
		}
		(*tccmp).lobyte = IENABLE|RNUM|GO;
		return;
	}
	tcdtp = &amp;TCADDR-&gt;tcdt;
	switch (tctab.d_active) {

	case SIO:
	done:
		tctab.d_active = 0;
		if (tctab.d_actf = bp-&gt;av_forw)
			tcstart();
		else
			TCADDR-&gt;tccm.lobyte = SAT|GO;
		iodone(bp);
		return;

	case SFORW:
		if (*tcdtp &gt; bp-&gt;b_blkno)
			goto setback;
		if (*tcdtp &lt; bp-&gt;b_blkno)
			goto setforw;
		*--tcdtp = bp-&gt;b_addr;		/* core address */
		*--tcdtp = bp-&gt;b_wcount;
		tccmp-&gt;lobyte = ((bp-&gt;b_xmem &amp; 03) &lt;&lt; 4) | IENABLE|GO
		    | (bp-&gt;b_flags&amp;B_READ?RDATA:WDATA);
		tctab.d_active = SIO;
		return;

	case SREV:
		if (*tcdtp+3 &gt; bp-&gt;b_blkno)
			goto setback;
		goto setforw;
	}
}
