#
/*
 */

/*
 * TM tape driver
 */

#include &quot;../param.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;

struct {
	int tmer;
	int tmcs;
	int tmbc;
	int tmba;
	int tmdb;
	int tmrd;
};

struct	devtab	tmtab;
struct	buf	rtmbuf;

char	t_openf[8];
char	*t_blkno[8];
char	*t_nxrec[8];

#define	TMADDR	0172520

#define	GO	01
#define	RCOM	02
#define	WCOM	04
#define	WEOF	06
#define	SFORW	010
#define	SREV	012
#define	WIRG	014
#define	REW	016
#define	DENS	060000		/* 9-channel */
#define	IENABLE	0100
#define	CRDY	0200
#define GAPSD	010000
#define	TUR	1
#define	HARD	0102200	/* ILC, EOT, NXM */
#define	EOF	0040000

#define	SSEEK	1
#define	SIO	2

tmopen(dev, flag)
{
	register dminor;

	dminor = dev.d_minor;
	if (t_openf[dminor])
		u.u_error = ENXIO;
	else {
		t_openf[dminor]++;
		t_blkno[dminor] = 0;
		t_nxrec[dminor] = 65535;
	}
}

tmclose(dev, flag)
{
	register int dminor;

	dminor = dev.d_minor;
	t_openf[dminor] = 0;
	if (flag)
		tcommand(dminor, WEOF);
	tcommand(dminor, REW);
}

tcommand(unit, com)
{
	extern lbolt;

	while (tmtab.d_active || (TMADDR-&gt;tmcs &amp; CRDY)==0)
		sleep(&amp;lbolt, 1);
	TMADDR-&gt;tmcs = DENS|com|GO | (unit&lt;&lt;8);
}

tmstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;
	register char **p;

	bp = abp;
	p = &amp;t_nxrec[bp-&gt;b_dev.d_minor];
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
	if (tmtab.d_actf==0)
		tmtab.d_actf = bp;
	else
		tmtab.d_actl-&gt;av_forw = bp;
	tmtab.d_actl = bp;
	if (tmtab.d_active==0)
		tmstart();
	spl0();
}

tmstart()
{
	register struct buf *bp;
	register int com;
	int unit;
	register char *blkno;

    loop:
	if ((bp = tmtab.d_actf) == 0)
		return;
	unit = bp-&gt;b_dev.d_minor;
	blkno = t_blkno[unit];
	if (t_openf[unit] &lt; 0 || (TMADDR-&gt;tmcs &amp; CRDY)==0) {
		bp-&gt;b_flags =| B_ERROR;
		tmtab.d_actf = bp-&gt;av_forw;
		iodone(bp);
		goto loop;
	}
	com = (unit&lt;&lt;8) | ((bp-&gt;b_xmem &amp; 03) &lt;&lt; 4) | IENABLE|DENS;
	if (blkno != bp-&gt;b_blkno) {
		tmtab.d_active = SSEEK;
		if (blkno &lt; bp-&gt;b_blkno) {
			com =| SFORW|GO;
			TMADDR-&gt;tmbc = blkno - bp-&gt;b_blkno;
		} else {
			if (bp-&gt;b_blkno == 0)
				com =| REW|GO;
			else {
				com =| SREV|GO;
				TMADDR-&gt;tmbc = bp-&gt;b_blkno - blkno;
			}
		}
		TMADDR-&gt;tmcs = com;
		return;
	}
	tmtab.d_active = SIO;
	TMADDR-&gt;tmbc = bp-&gt;b_wcount &lt;&lt; 1;
	TMADDR-&gt;tmba = bp-&gt;b_addr;		/* core address */
	TMADDR-&gt;tmcs = com | ((bp-&gt;b_flags&amp;B_READ)? RCOM|GO:
	    ((tmtab.d_errcnt)? WIRG|GO: WCOM|GO));
}

tmintr()
{
	register struct buf *bp;
	register int unit;

	if ((bp = tmtab.d_actf)==0)
		return;
	unit = bp-&gt;b_dev.d_minor;
	if (TMADDR-&gt;tmcs &lt; 0) {		/* error bit */
/*
		deverror(bp, TMADDR-&gt;tmer);
 */
		while(TMADDR-&gt;tmrd &amp; GAPSD) ; /* wait for gap shutdown */
		if ((TMADDR-&gt;tmer&amp;(HARD|EOF))==0 &amp;&amp; tmtab.d_active==SIO) {
			if (++tmtab.d_errcnt &lt; 10) {
				t_blkno[unit]++;
				tmtab.d_active = 0;
				tmstart();
				return;
			}
		} else
			if(bp != &amp;rtmbuf &amp;&amp; (TMADDR-&gt;tmer&amp;EOF)==0)
				t_openf[unit] = -1;
		bp-&gt;b_flags =| B_ERROR;
		tmtab.d_active = SIO;
	}
	if (tmtab.d_active == SIO) {
		tmtab.d_errcnt = 0;
		t_blkno[unit]++;
		tmtab.d_actf = bp-&gt;av_forw;
		tmtab.d_active = 0;
		iodone(bp);
		bp-&gt;b_resid = TMADDR-&gt;tmbc;
	} else
		t_blkno[unit] = bp-&gt;b_blkno;
	tmstart();
}

tmread(dev)
{
	tmphys(dev);
	physio(tmstrategy, &amp;rtmbuf, dev, B_READ);
	u.u_count = -rtmbuf.b_resid;
}

tmwrite(dev)
{
	tmphys(dev);
	physio(tmstrategy, &amp;rtmbuf, dev, B_WRITE);
	u.u_count = 0;
}

tmphys(dev)
{
	register unit, a;

	unit = dev.d_minor;
	a = lshift(u.u_offset, -9);
	t_blkno[unit] = a;
	t_nxrec[unit] = ++a;
}
