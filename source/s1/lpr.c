/*
 *	dpr -- off line print via dataphone daemon to GCOS
 *		normally invoked through opr
 */

char	tfname[]	&quot;/usr/lpd/tfaXXXXX&quot;;
char	cfname[]	&quot;/usr/lpd/cfaXXXXX&quot;;
char	lfname[]	&quot;/usr/lpd/lfaXXXXX&quot;;
char	dfname[]	&quot;/usr/lpd/dfaXXXXX&quot;;
int	nact;
int	tff;
int	mailflg;
char	person[10];
int	inchar;
int	maxrec	1000;

main(argc, argv)
int argc;
char *argv[];
{
	register char *arg;
	int c, f, flag;
	int out();

	pidfn();
	if((signal(1, 1) &amp; 01) == 0)
		signal(1, out);
	if((signal(2, 1) &amp; 01) == 0)
		signal(2, out);
	if((signal(3, 1) &amp; 01) == 0)
		signal(3, out);
	flag = 0;
	tff = nfile(tfname);
	while (argc&gt;1 &amp;&amp; (arg = argv[1])[0]==&#39;-&#39;) {
		switch (arg[1]) {

		case &#39;c&#39;:
			flag = &#39;+&#39;;
			break;

		case &#39;r&#39;:
			flag = &#39;-&#39;;
			break;

		case &#39;m&#39;:
			mailflg = 1;
			break;
		}
		argc--;
		argv++;
	}
	ident();

	if(argc == 1)
		copy(0);
	while(--argc) {
		arg = *++argv;
		if(flag == &#39;+&#39;)
			goto cf;
		if(*arg == &#39;/&#39; &amp;&amp; flag != &#39;-&#39;) {
			card(&#39;F&#39;, arg);
			nact++;
			continue;
		}
		if(link(arg, lfname) &lt; 0)
			goto cf;
		card(&#39;F&#39;, lfname);
		card(&#39;U&#39;, lfname);
		lfname[inchar]++;
		nact++;
		goto df;

	cf:
		f = open(arg, 0);
		if(f &lt; 0) {
			printf(&quot;Cannot open %s\n&quot;, arg);
			continue;
		}
		copy(f);
		close(f);

	df:
		if(flag == &#39;-&#39;) {
			f = unlink(arg);
			if(f &lt; 0)
				printf(&quot;Cannot remove %s\n&quot;, arg);
		}
	}

	if(nact) {
		tfname[inchar]--;
		f = link(tfname, dfname);
		if(f &lt; 0) {
			printf(&quot;Cannot rename %s\n&quot;, dfname);
			tfname[inchar]++;
			out();
		}
		unlink(tfname);
		execl(&quot;/etc/lpd&quot;, &quot;lpd&quot;, 0);
		dfname[inchar]++;
	}
	out();
}

copy(f)
int f;
{
	int ff, i, nr, nc;
	static int buf[256];

	card(&#39;F&#39;, cfname);
	card(&#39;U&#39;, cfname);
	ff = nfile(cfname);
	nc = 0;
	nr = 0;
	while((i = read(f, buf, 512)) &gt; 0) {
		write(ff, buf, i);
		nc =+ i;
		if(nc &gt;= 512) {
			nc =- 512;
			nr++;
			if(nr &gt; maxrec) {
				printf(&quot;Copy file is too large\n&quot;);
				break;
			}
		}
	}
	close(ff);
	nact++;
}

card(c, s)
int c;
char s[];
{
	char *p1, *p2;
	static char buf[512];
	int col;

	p1 = buf;
	p2 = s;
	col = 0;
	*p1++ = c;
	while((c = *p2++) != &#39;\0&#39;) {
		*p1++ = c;
		col++;
	}
	*p1++ = &#39;\n&#39;;
	write(tff, buf, col+2);
}

ident()
{
	int c, n;
	register char *b1p, *pp, *b2p;
	static char b1[100], b2[100];

	b1p = b1;
	if(getpw(getuid(), b1p)) {
		b1p = &quot;pdp::::m0000,m000:&quot;;
	}
	n = 0;
	b2p = b2;
	while(*b2p++ = &quot;$	ident	&quot;[n++]);
	b2p--;
	n = 5;
	while(--n) while(*b1p++ != &#39;:&#39;);
	while((*b2p++ = *b1p++) != &#39;:&#39;);
	b2p[-1] = &#39;,&#39;;
	b1p = b1;
	pp = person;
	while((c = *b1p++) != &#39;:&#39;) {
		*b2p++ = c;
		*pp++ = c;
	}
	*b2p++ = 0;
	*pp++ = 0;
	card(&#39;L&#39;, b2);
	if (mailflg)
		card(&#39;M&#39;, person);
}

pidfn()
{
	register i, j, c;
	int p;

	p = getpid();
	i = 0;
	while(tfname[i] != &#39;X&#39;)
		i++;
	i =+ 4;
	for(j=0; j&lt;5; j++) {
		c = (p%10) + &#39;0&#39;;
		p =/ 10;
		tfname[i] = c;
		cfname[i] = c;
		lfname[i] = c;
		dfname[i] = c;
		i--;
	}
	inchar = i;
}

nfile(name)
char *name;
{
	register f;

	f = creat(name, 0666);
	if(f &lt; 0) {
		printf(&quot;Cannot create %s\n&quot;, name);
		out();
	}
	name[inchar]++;
	return(f);
}

out()
{
	register i;

	signal(1, 1);
	signal(1, 2);
	signal(1, 3);
	i = inchar;
	while(tfname[i] != &#39;a&#39;) {
		tfname[i]--;
		unlink(tfname);
	}
	while(cfname[i] != &#39;a&#39;) {
		cfname[i]--;
		unlink(cfname);
	}
	while(lfname[i] != &#39;a&#39;) {
		lfname[i]--;
		unlink(lfname);
	}
	while(dfname[i] != &#39;a&#39;) {
		dfname[i]--;
		unlink(dfname);
	}
	exit();
}
