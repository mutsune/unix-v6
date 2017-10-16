#
/*
 */

/*
 *	DM-BB driver
 */
#include &quot;../param.h&quot;
#include &quot;../tty.h&quot;
#include &quot;../conf.h&quot;

#define	DMADDR	0170500

struct	tty dh11[];
int	ndh11;		/* Set by dh.c to number of lines */

#define	DONE	0200
#define	SCENABL	040
#define	CLSCAN	01000
#define	TURNON	07	/* RQ send, CD lead, line enable */
#define	TURNOFF	1	/* line enable only */
#define	CARRIER	0100

struct dmregs {
	int	dmcsr;
	int	dmlstat;
};

/*
 * Turn on the line associated with the (DH) device dev.
 */
dmopen(dev)
{
	register struct tty *tp;

	tp = &amp;dh11[dev.d_minor];
	DMADDR-&gt;dmcsr = dev.d_minor;
	DMADDR-&gt;dmlstat = TURNON;
	if (DMADDR-&gt;dmlstat&amp;CARRIER)
		tp-&gt;t_state =| CARR_ON;
	DMADDR-&gt;dmcsr = IENABLE|SCENABL;
	spl5();
	while ((tp-&gt;t_state&amp;CARR_ON)==0)
		sleep(&amp;tp-&gt;t_rawq, TTIPRI);
	spl0();
}

/*
 * If a DH line has the HUPCL mode,
 * turn off carrier when it is closed.
 */
dmclose(dev)
{
	register struct tty *tp;

	tp = &amp;dh11[dev.d_minor];
	if (tp-&gt;t_flags&amp;HUPCL) {
		DMADDR-&gt;dmcsr = dev.d_minor;
		DMADDR-&gt;dmlstat = TURNOFF;
		DMADDR-&gt;dmcsr = IENABLE|SCENABL;
	}
}

/*
 * DM11 interrupt.
 * Mainly, deal with carrier transitions.
 */
dmint()
{
	register struct tty *tp;

	if (DMADDR-&gt;dmcsr&amp;DONE) {
		tp = &amp;dh11[DMADDR-&gt;dmcsr&amp;017];
		if (tp &lt; &amp;dh11[ndh11]) {
			wakeup(tp);
			if ((DMADDR-&gt;dmlstat&amp;CARRIER)==0) {
				if ((tp-&gt;t_state&amp;WOPEN)==0) {
					signal(tp, SIGHUP);
					DMADDR-&gt;dmlstat = 0;
					flushtty(tp);
				}
				tp-&gt;t_state =&amp; ~CARR_ON;
			} else
				tp-&gt;t_state =| CARR_ON;
		}
		DMADDR-&gt;dmcsr = IENABLE|SCENABL;
	}
}
