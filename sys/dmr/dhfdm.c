#
/*
 */

/*
 *	DM-BB fake driver
 */
#include &quot;../tty.h&quot;
#include &quot;../conf.h&quot;

struct	tty	dh11[];

dmopen(dev)
{
	register struct tty *tp;

	tp = &amp;dh11[dev.d_minor];
	tp-&gt;t_state =| CARR_ON;
}

dmclose(dev)
{
}
