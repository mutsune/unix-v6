#
/*
 */

/*
 *	indirect driver for controlling tty.
 */
#include &quot;../param.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../user.h&quot;
#include &quot;../tty.h&quot;
#include &quot;../proc.h&quot;

syopen(dev, flag)
{
	register *tp;

	if(tp = syttyp())
	(*cdevsw[tp-&gt;t_dev.d_major].d_open)(tp-&gt;t_dev, flag);
}

syread(dev)
{
	register *tp;

	if(tp = syttyp())
	(*cdevsw[tp-&gt;t_dev.d_major].d_read)(tp-&gt;t_dev);
}

sywrite(dev)
{
	register *tp;

	if(tp = syttyp())
	(*cdevsw[tp-&gt;t_dev.d_major].d_write)(tp-&gt;t_dev);
}

sysgtty(dev, flag)
{
	register *tp;

	if(tp = syttyp())
	(*cdevsw[tp-&gt;t_dev.d_major].d_sgtty)(tp-&gt;t_dev, flag);
}

syttyp()
{
	register tp;

	tp = u.u_procp-&gt;p_ttyp;
	if(tp == NULL)
		u.u_error = ENXIO;
	return(tp);
}
