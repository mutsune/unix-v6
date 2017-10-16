/*
 * mesg -- set current tty to accept or
 *	forbid write permission.
 *
 *	mesg [y] [n]
 *		y allow messages
 *		n forbid messages
 */

int	sbuf[40];

main(argc, argv)
char *argv[];
{
	register char *tty;

	tty = &quot;/dev/ttyx&quot;;
	tty[8] = ttyn(1);
	if(stat(tty, sbuf) &lt; 0) {
		write(2, &quot;cannot stat\n&quot;, 12);
		exit(1);
	}
	if(argc &lt; 2) {
		if(sbuf[2] &amp; 02)
			goto no;
		goto yes;
	}
	if(*argv[1] == &#39;y&#39;)
		goto yes;

no:
	if(chmod(tty, 0600) &lt; 0)
		goto bad;
	goto was;

yes:
	if(chmod(tty, 0622) &lt; 0)
		goto bad;

was:
	if(sbuf[2] &amp; 02)
		write(2, &quot;was y\n&quot;, 6); else
		write(2, &quot;was n\n&quot;, 6);
	exit(0);

bad:
	write(2, &quot;cannot change mode\n&quot;, 19);
	exit(1);
}
