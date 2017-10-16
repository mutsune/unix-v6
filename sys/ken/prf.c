#
/*
 */

#include &quot;../param.h&quot;
#include &quot;../seg.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../conf.h&quot;

/*
 * Address and structure of the
 * KL-11 console device registers.
 */
struct
{
	int	rsr;
	int	rbr;
	int	xsr;
	int	xbr;
};

/*
 * In case console is off,
 * panicstr contains argument to last
 * call to panic.
 */

char	*panicstr;

/*
 * Scaled down version of C Library printf.
 * Only %s %l %d (==%l) %o are recognized.
 * Used to print diagnostic information
 * directly on console tty.
 * Since it is not interrupt driven,
 * all system activities are pretty much
 * suspended.
 * Printf should not be used for chit-chat.
 */
printf(fmt,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc)
char fmt[];
{
	register char *s;
	register *adx, c;

	adx = &amp;x1;
loop:
	while((c = *fmt++) != &#39;%&#39;) {
		if(c == &#39;\0&#39;)
			return;
		putchar(c);
	}
	c = *fmt++;
	if(c == &#39;d&#39; || c == &#39;l&#39; || c == &#39;o&#39;)
		printn(*adx, c==&#39;o&#39;? 8: 10);
	if(c == &#39;s&#39;) {
		s = *adx;
		while(c = *s++)
			putchar(c);
	}
	adx++;
	goto loop;
}

/*
 * Print an unsigned integer in base b.
 */
printn(n, b)
{
	register a;

	if(a = ldiv(n, b))
		printn(a, b);
	putchar(lrem(n, b) + &#39;0&#39;);
}

/*
 * Print a character on console.
 * Attempts to save and restore device
 * status.
 * If the switches are 0, all
 * printing is inhibited.
 */
putchar(c)
{
	register rc, s;

	rc = c;
	if(SW-&gt;integ == 0)
		return;
	while((KL-&gt;xsr&amp;0200) == 0)
		;
	if(rc == 0)
		return;
	s = KL-&gt;xsr;
	KL-&gt;xsr = 0;
	KL-&gt;xbr = rc;
	if(rc == &#39;\n&#39;) {
		putchar(&#39;\r&#39;);
		putchar(0177);
		putchar(0177);
	}
	putchar(0);
	KL-&gt;xsr = s;
}

/*
 * Panic is called on unresolvable
 * fatal errors.
 * It syncs, prints &quot;panic: mesg&quot; and
 * then loops.
 */
panic(s)
char *s;
{
	panicstr = s;
	update();
	printf(&quot;panic: %s\n&quot;, s);
	for(;;)
		idle();
}

/*
 * prdev prints a warning message of the
 * form &quot;mesg on dev x/y&quot;.
 * x and y are the major and minor parts of
 * the device argument.
 */
prdev(str, dev)
{

	printf(&quot;%s on dev %l/%l\n&quot;, str, dev.d_major, dev.d_minor);
}

/*
 * deverr prints a diagnostic from
 * a device driver.
 * It prints the device, block number,
 * and an octal word (usually some error
 * status register) passed as argument.
 */
deverror(bp, o1, o2)
int *bp;
{
	register *rbp;

	rbp = bp;
	prdev(&quot;err&quot;, rbp-&gt;b_dev);
	printf(&quot;bn%l er%o %o\n&quot;, rbp-&gt;b_blkno, o1, o2);
}
