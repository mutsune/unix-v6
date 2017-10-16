#
/*
 */

/*
 * VT01 driver via DR11C to 11/20
 */

#include &quot;../param.h&quot;
#include &quot;../user.h&quot;

int	vtflag;

struct vtreg {
	int	csr;
	int	buf;
};

#define	VTADDR	0167770
#define	RQINT	01
#define	BIENABL	040
#define	SEOF	0100000
#define	VTPRI	8

vtopen(dev, flag)
{
	if (!flag)
		u.u_error = ENXIO;
	else
		VTADDR-&gt;csr = BIENABL;
}

vtclose()
{
	VTADDR-&gt;buf = SEOF;
	VTADDR-&gt;csr =| RQINT;
}

vtwrite()
{
	register int c;
	int register count;

	while ((c=cpass()) &gt;= 0) {
	    retry:
		for (count=0; count&lt;10; count++)
			if ((VTADDR-&gt;csr&amp;RQINT)==0) {
				VTADDR-&gt;buf = c&amp;0377;
				VTADDR-&gt;csr =| RQINT;
				goto contin;
			}
		spl5();
		if (VTADDR-&gt;csr&amp;RQINT) {
			vtflag++;
			sleep(VTADDR, VTPRI);
		}
		spl0();
		goto retry;
    contin:;
	}
}

vtintr()
{
	VTADDR-&gt;csr =&amp; ~RQINT;
	if (vtflag) {
		vtflag = 0;
		wakeup(VTADDR);
	}
}
