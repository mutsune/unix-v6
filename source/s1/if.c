/* if command */

int	ap;
int	ac;
char	**av;

main(argc, argv)
char *argv[];
{

	argv[argc] = 0;
	ac = argc; av = argv; ap = 1;
	if (argc&lt;2) return;
	if (exp()) 
		if(doex(0)) {
			write(1, &quot;no command\n&quot;, 11);
			seek(0, 0, 2);
		}
}

char *nxtarg() {

	if (ap&gt;ac) return(0*ap++);
	return(av[ap++]);
}

exp(s) {
	int p1;

	p1 = e1();
	if (eq(nxtarg(), &quot;-o&quot;)) return(p1 | exp());
	ap--;
	return(p1);
}

e1() {
	int p1;

	p1 = e2();
	if (eq(nxtarg(), &quot;-a&quot;)) return (p1 &amp; e1());
	ap--;
	return(p1);
}

e2() {
	if (eq(nxtarg(), &quot;!&quot;))
		return(!e3());
	ap--;
	return(e3());
}

e3() {
	int p1, ccode;
	register char *a;

	ccode = 0;
	if ((a=nxtarg())==0) goto err;
	if(eq(a, &quot;(&quot;)) {
		p1 = exp();
		if(!eq(nxtarg(), &quot;)&quot;)) goto err;
		return(p1);
	}

	if(eq(a, &quot;-r&quot;))
		return(tio(nxtarg(), 0));

	if(eq(a, &quot;-w&quot;))
		return(tio(nxtarg(), 1));

	if(eq(a, &quot;-c&quot;))
		return(tcreat(nxtarg()));

	if(eq(a, &quot;{&quot;)) { /* execute a command for exit code */
		if(fork()) /*parent*/ wait(&amp;ccode);
		else { /*child*/
			doex(1);
			goto err;
		}
		while((a=nxtarg()) &amp;&amp; (!eq(a,&quot;}&quot;)));
		return(ccode? 0 : 1);
	}

	p1 = nxtarg();
	if (p1==0) goto err;
	if(eq(p1, &quot;=&quot;))
		return(eq(a, nxtarg()));

	if(eq(p1, &quot;!=&quot;))
		return(!eq(a, nxtarg()));
err:
	write(1, &quot;if error\n&quot;, 9);
	exit(9);
}

tio(a, f) {

	a = open(a, f);
	if (a&gt;=0) {
		close(a);
		return(1);
	}
	return(0);
}

tcreat(a) {
	return(1);
}

eq(a, b)
char *a, *b;
{
	register int i;

	i = 0;
l:
	if(a[i] != b[i])
		return(0);
	if(a[i++] == &#39;\0&#39;)
		return(1);
	goto l;
}

doex(earg) {

	register int np, i, c;
	char *nargv[50], *ncom, *na;

	np = 0;
	while (na=nxtarg()) {
		if(earg &amp;&amp; eq(na,&quot;}&quot;)) break;
		nargv[np++] = na;
	}
	if(earg &amp;&amp; (!eq(na, &quot;}&quot;))) return(9);
	nargv[np] = 0;
	if (np==0) return(earg);
	execv(nargv[0], nargv, np);
	i = 0;
	ncom = &quot;/usr/bin/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx&quot;;
	while(c=nargv[0][i])  {
		ncom[9+i++] = c;
	}
	ncom[9+i] = &#39;\0&#39;;
	execv(ncom+4, nargv, np);
	execv(ncom, nargv, np);
	return(1);
}
