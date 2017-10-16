#
/*
 */

/*
 * Screw Works interface via DC-11
 */

#include &quot;../tty.h&quot;

#define	VSADDR	0174150
#define	CDLEAD	01
#define	B1200	030
#define	STOP1	0400
#define	CLSEND	02
#define	RQSEND	01

#define	MAGIC_MAP 0377

struct {
	int	vsrcsr;
	int	vsrbuf;
	int	vsxcsr;
	int	vsxbuf;
};

struct {
	struct	clist	iq;
	struct	clist	oq;
} vs;

vsopen(dev)
{
	VSADDR-&gt;vsrcsr = IENABLE|B1200|CDLEAD;
	VSADDR-&gt;vsxcsr = STOP1|IENABLE|B1200;
	vschar(0);
}

vsclose(dev)
{
	vschar(0);
	VSADDR-&gt;vsrcsr =&amp; ~IENABLE;
	while (getc(&amp;vs.iq) &gt;= 0);
}

vswrite(dev)
{
	register int count, c;

	count = 0;
	while ((c=cpass()) &gt;= 0) {
		if (--count &lt;= 0) {
			count = 60;
			vschar(0);
		}
		vschar(c);
	}
	vschar(0);
}

vschar(c)
{

	c =^ MAGIC_MAP;
	spl5();
	while (vs.oq.c_cc &gt; 60) {
		vsxintr();
		sleep(&amp;vs.oq, TTIPRI);
	}
	putc(c, &amp;vs.oq);
	vsxintr();
	spl0();
}

vsxintr()
{
	static lchar;
	register c;
	register int *xcsr;

	xcsr = &amp;VSADDR-&gt;vsxcsr;
	if (*xcsr&amp;DONE) {
		if (lchar==MAGIC_MAP) {
			*xcsr =&amp; ~RQSEND;
			lchar = 0;
			if (vs.oq.c_cc==0)
				goto wake;
		}
		if ((*xcsr&amp;CLSEND) == 0) {
			*xcsr =&amp; ~RQSEND;
			*xcsr =| RQSEND;
			if ((*xcsr&amp;CLSEND) == 0)
				goto wake;
		}
		if ((c = getc(&amp;vs.oq)) &gt;= 0)
			VSADDR-&gt;vsxbuf = lchar = c;
		if (vs.oq.c_cc &lt;= 15)
	    wake:
			wakeup(&amp;vs.oq);
	}
}

vsread(dev)
{
	register int c;

	spl5();
	while ((c = getc(&amp;vs.iq)) &lt; 0)
		sleep(&amp;vs.iq, TTIPRI);
	spl0();
	passc(&quot;?0*#?546?213?879?&quot;[c&amp;017]);
}

vsrintr()
{
	register int c;

	c = VSADDR-&gt;vsrbuf;
	if (vs.iq.c_cc&lt;=10)
		putc(c, &amp;vs.iq);
	wakeup(&amp;vs.iq);
}
