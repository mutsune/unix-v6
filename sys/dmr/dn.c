#
/*
 */

/*
 * DN-11 ACU interface
 */

#include &quot;../param.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;

struct dn {
	struct {
		char	dn_stat;
		char	dn_reg;
	} dn11[3];
}

#define	DNADDR	0175200

#define	PWI	00200
#define	ACR	00100
#define	DLO	0020
#define	DONE	0200
#define	IENABLE	0100
#define	DSS	040
#define	PND	020
#define	MENABLE	04
#define	DPR	02
#define	CRQ	01

#define	DNPRI	5

dnopen(dev, flag)
{
	register struct dn *dp;
	register int rdev;

	rdev = dev.d_minor;
	dp = &amp;DNADDR-&gt;dn11[rdev];
	if (dp-&gt;dn_reg&amp;(PWI|DLO))
		u.u_error = ENXIO;
	else {
		DNADDR-&gt;dn11[0].dn_stat =| MENABLE;
		dp-&gt;dn_stat = IENABLE|MENABLE|CRQ;
	}
}

dnclose(dev)
{
	DNADDR-&gt;dn11[dev.d_minor].dn_stat =&amp; MENABLE;
}

dnwrite(dev)
{
	register struct dn *dp;
	register c;
	extern lbolt;

	dp = &amp;DNADDR-&gt;dn11[dev.d_minor];
	for(;;) {
		while ((dp-&gt;dn_stat&amp;DONE)==0)
			sleep(DNADDR, DNPRI);
		dp-&gt;dn_stat =&amp; ~DONE;
	    contin:
		if (dp-&gt;dn_reg&amp;(PWI|ACR)) {
			u.u_error = EIO;
			return;
		}
		if (dp-&gt;dn_stat&amp;DSS)
			return;
		c = 0;
		if (u.u_count==0 || (dp-&gt;dn_stat&amp;PND)==0 || (c=cpass())&lt;0)
			continue;
		if (c==&#39;-&#39;) {
			sleep(&amp;lbolt, DNPRI);
			sleep(&amp;lbolt, DNPRI);
			goto contin;
		}
		dp-&gt;dn_reg = c-&#39;0&#39;;
		dp-&gt;dn_stat =| DPR;
	}
}

dnint(dev)
{
	wakeup(DNADDR);
}
