#
/*
 */

/*
 * GP DR11C driver used for C/A/T
 */

#include &quot;../param.h&quot;
#include &quot;../user.h&quot;
#include &quot;../tty.h&quot;

#define	CATADDR	0167750
#define	PCAT	9
#define	CATHIWAT	60
#define	CATLOWAT	15

struct {
	int	catlock;
	struct	clist	oq;
} cat;

struct {
	int	catcsr;
	int	catbuf;
};

ctopen(dev)
{
	if (cat.catlock==0) {
		cat.catlock++;
		CATADDR-&gt;catcsr =| IENABLE;
	} else
		u.u_error = ENXIO;
}

ctclose()
{
	cat.catlock = 0;
}

ctwrite(dev)
{
	register c;
	extern lbolt;

	while ((c=cpass()) &gt;= 0) {
		spl5();
		while (cat.oq.c_cc &gt; CATHIWAT)
			sleep(&amp;cat.oq, PCAT);
		while (putc(c, &amp;cat.oq) &lt; 0)
			sleep(&amp;lbolt, PCAT);
		catintr();
		spl0();
	}
}

catintr()
{
	register int c;

	if (CATADDR-&gt;catcsr&amp;DONE &amp;&amp; (c=getc(&amp;cat.oq))&gt;=0) {
		CATADDR-&gt;catbuf = c;
		if (cat.oq.c_cc==0 || cat.oq.c_cc==CATLOWAT)
			wakeup(&amp;cat.oq);
	} else {
		if (cat.catlock==0)
			CATADDR-&gt;catcsr = 0;
	}
}
