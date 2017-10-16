/* Fortran command */

char	*tmp;
char ts[1000];
char *tsp ts;
char *av[50];
char *clist[50];
char *llist[50];
int instring;
int pflag;
int cflag;
char	*complr;
int *ibuf;
int *ibuf1;
int *ibuf2;
int *obuf;
char *lp;
char *line;
int lineno;
int exfail;
struct symtab {
	char name[8];
	char *value;
} *symtab;
int symsiz 200;
struct symtab *defloc;
struct symtab *incloc;
char *stringbuf;

main(argc, argv)
char *argv[]; {
	char *t;
	int nc, nl, i, j, c, nxo;
	int dexit();

	complr = &quot;/usr/fort/fc1&quot;;
	i = nc = nl = nxo = 0;
	while(++i &lt; argc) {
		if(*argv[i] == &#39;-&#39;)
			switch (argv[i][1]) {
				default:
					goto passa;
				case &#39;p&#39;:
					pflag++;
				case &#39;c&#39;:
					cflag++;
					break;
				case &#39;2&#39;:
					complr = &quot;/usr/fort/fc2&quot;;
					break;
			}
		else {
		passa:
			t = argv[i];
			if(getsuf(t)==&#39;f&#39;) {
				clist[nc++] = t;
				t = setsuf(copy(t), &#39;o&#39;);
			}
			if (nodup(llist, t)) {
				llist[nl++] = t;
				if (getsuf(t)==&#39;o&#39;)
					nxo++;
			}
		}
	}
	if(nc==0)
		goto nocom;
	if ((signal(2, 1) &amp; 01) == 0)
		signal(2, &amp;dexit);
	for (i=0; i&lt;nc; i++) {
		if (nc&gt;1)
			printf(&quot;%s:\n&quot;, clist[i]);
		tmp = 0;
		av[0] = complr;
		av[1] = expand(clist[i]);
		if (pflag || exfail)
			continue;
		if (av[1] == 0) {
			cflag++;
			continue;
		}
		av[2] = 0;
		t = callsys(complr, av);
		if(tmp)
			cunlink(tmp);
		if(t) {
			cflag++;
			continue;
		}
		av[0] = &quot;as&quot;;
		av[1] = &quot;-&quot;;
		av[2] = &quot;f.tmp1&quot;;
		av[3] = 0;
		callsys(&quot;/bin/as&quot;, av);
		t = setsuf(clist[i], &#39;o&#39;);
		cunlink(t);
		if(link(&quot;a.out&quot;, t) || cunlink(&quot;a.out&quot;)) {
			printf(&quot;move failed: %s\n&quot;, t);
			cflag++;
		}
	}
nocom:
	if (cflag==0 &amp;&amp; nl!=0) {
		i = 0;
		av[0] = &quot;ld&quot;;
		av[1] = &quot;-x&quot;;
		av[2] = &quot;/lib/fr0.o&quot;;
		j = 3;
		while(i&lt;nl)
			av[j++] = llist[i++];
		av[j++] = &quot;-lf&quot;;
		av[j++] = &quot;/lib/filib.a&quot;;
		av[j++] = &quot;-l&quot;;
		av[j++] = 0;
		callsys(&quot;/bin/ld&quot;, av);
		if (nc==1 &amp;&amp; nxo==1)
			cunlink(setsuf(clist[0], &#39;o&#39;));
	}
	dexit();
}

dexit()
{
	unlink(&quot;f.tmp1&quot;);
	exit();
}

expand(file)
char *file;
{
	int ib1[259], ib2[259], ob[259];
	struct symtab stab[200];
	char ln[196], sbf[1024];
	int c;

	exfail = 0;
	ibuf = ibuf1 = ib1;
	ibuf2 = ib2;
	if (fopen(file, ibuf1)&lt;0)
		return(file);
	if (getc(ibuf1) != &#39;#&#39;) {
		close(ibuf1[0]);
		return(file);
	}
	ibuf1[1]++;
	ibuf1[2]--;
	obuf = ob;
	symtab = stab;
	for (c=0; c&lt;200; c++) {
		stab[c].name[0] = &#39;\0&#39;;
		stab[c].value = 0;
	}
	defloc = lookup(&quot;define&quot;, 1);
	defloc-&gt;value = defloc-&gt;name;
	incloc = lookup(&quot;include&quot;, 1);
	incloc-&gt;value = incloc-&gt;name;
	stringbuf = sbf;
	line  = ln;
	lineno = 0;
	tmp = setsuf(copy(file), &#39;i&#39;);
	if (fcreat(tmp, obuf) &lt; 0) {
		printf(&quot;Can&#39;t creat %s\n&quot;, tmp);
		dexit();
	}
	while(getline()) {
/*
		if (ibuf==ibuf2)
			putc(001, obuf);	/*SOH: insert */
		if (ln[0] != &#39;#&#39;)
			for (lp=line; *lp!=&#39;\0&#39;; lp++)
				putc(*lp, obuf);
		putc(&#39;\n&#39;, obuf);
	}
	fflush(obuf);
	close(obuf[0]);
	close(ibuf1[0]);
	return(tmp);
}

getline()
{
	int c, sc, state;
	struct symtab *np;
	char *namep, *filname;

	if (ibuf==ibuf1)
		lineno++;
	lp = line;
	*lp = &#39;\0&#39;;
	state = 0;
	if ((c=getch()) == &#39;#&#39;)
		state = 1;
	while (c!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;) {
		if (&#39;a&#39;&lt;=c &amp;&amp; c&lt;=&#39;z&#39; || &#39;A&#39;&lt;=c &amp;&amp; c&lt;=&#39;Z&#39; || c==&#39;_&#39;) {
			namep = lp;
			sch(c);
			while (&#39;a&#39;&lt;=(c=getch()) &amp;&amp; c&lt;=&#39;z&#39;
			      ||&#39;A&#39;&lt;=c &amp;&amp; c&lt;=&#39;Z&#39;
			      ||&#39;0&#39;&lt;=c &amp;&amp; c&lt;=&#39;9&#39; 
			      ||c==&#39;_&#39;)
				sch(c);
			sch(&#39;\0&#39;);
			lp--;
			np = lookup(namep, state);
			if (state==1) {
				if (np==defloc)
					state = 2;
				else if (np==incloc)
					state = 3;
				else {
					error(&quot;Undefined control&quot;);
					while (c!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;)
						c = getch();
					return(c);
				}
			} else if (state==2) {
				np-&gt;value = stringbuf;
				while ((c=getch())!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;)
					savch(c);
				savch(&#39;\0&#39;);
				return(1);
			}
			continue;
		} else if ((sc=c)==&#39;\&#39;&#39; || sc==&#39;&quot;&#39;) {
			sch(sc);
			filname = lp;
			instring++;
			while ((c=getch())!=sc &amp;&amp; c!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;) {
				sch(c);
				if (c==&#39;\\&#39;)
					sch(getch());
			}
			instring = 0;
			if (state==3) {
				*lp = &#39;\0&#39;;
				while ((c=getch())!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;);
				if (ibuf==ibuf2)
					error(&quot;Nested &#39;include&#39;&quot;);
				if (fopen(filname, ibuf2)&lt;0)
					error(&quot;Missing file %s&quot;, filname);
				else
					ibuf = ibuf2;
				return(c);
			}
		}
		sch(c);
		c = getch();
	}
	sch(&#39;\0&#39;);
	if (state&gt;1)
		error(&quot;Control syntax&quot;);
	return(c);
}

error(s, x)
{
	printf(&quot;%d: &quot;, lineno);
	printf(s, x);
	putchar(&#39;\n&#39;);
	exfail++;
	cflag++;
}

sch(c)
{
	if (lp==line+194)
		error(&quot;Line overflow&quot;);
	*lp++ = c;
	if (lp&gt;line+195)
		lp = line+195;
}

savch(c)
{
	*stringbuf++ = c;
}

getch()
{
	static peekc;
	int c;

	if (peekc) {
		c = peekc;
		peekc = 0;
		return(c);
	}
loop:
	if ((c=getc1())==&#39;/&#39; &amp;&amp; !instring) {
		if ((peekc=getc1())!=&#39;*&#39;)
			return(&#39;/&#39;);
		peekc = 0;
		for(;;) {
			c = getc1();
		cloop:
			switch (c) {

			case &#39;\0&#39;:
				return(&#39;\0&#39;);

			case &#39;*&#39;:
				if ((c=getc1())==&#39;/&#39;)
					goto loop;
				goto cloop;

			case &#39;\n&#39;:
				if (ibuf==ibuf1) {
					putc(&#39;\n&#39;, obuf);
					lineno++;
				}
				continue;
			}
		}
	}
	return(c);
}

getc1()
{
	int c;

	if ((c = getc(ibuf)) &lt; 0 &amp;&amp; ibuf==ibuf2) {
		close(ibuf2[0]);
		ibuf = ibuf1;
		putc(&#39;\n&#39;, obuf);
		c = getc1();
	}
	if (c&lt;0)
		return(0);
	return(c);
}

lookup(namep, enterf)
char *namep;
{
	char *np, *snp;
	struct symtab *sp;
	int i, c;

	np = namep;
	i = 0;
	while (c = *np++)
		i =+ c;
	i =% symsiz;
	sp = &amp;symtab[i];
	while (sp-&gt;name[0]) {
		snp = sp;
		np = namep;
		while (*snp++ == *np)
			if (*np++ == &#39;\0&#39; || np==namep+8) {
				if (!enterf)
					subst(namep, sp);
				return(sp);
			}
		if (sp++ &gt; &amp;symtab[symsiz])
			sp = symtab;
	}
	if (enterf) {
		for (i=0; i&lt;8; i++)
			if (sp-&gt;name[i] = *namep)
				namep++;
		while (*namep)
			namep++;
	}
	return(sp);
}

subst(np, sp)
char *np;
struct symtab *sp;
{
	char *vp;

	lp = np;
	if ((vp = sp-&gt;value) == 0)
		return;
	sch(&#39; &#39;);
	while (*vp)
		sch(*vp++);
	sch(&#39; &#39;);
}

getsuf(s)
char s[];
{
	int c;
	char t, *os;

	c = 0;
	os = s;
	while(t = *s++)
		if (t==&#39;/&#39;)
			c = 0;
		else
			c++;
	s =- 3;
	if (c&lt;=14 &amp;&amp; c&gt;2 &amp;&amp; *s++==&#39;.&#39;)
		return(*s);
	return(0);
}

setsuf(s, ch)
char s[];
{
	char *os;

	os = s;
	while(*s++);
	s[-2] = ch;
	return(os);
}

callsys(f, v)
char f[], *v[]; {
	int t, status;

	if ((t=fork())==0) {
		execv(f, v);
		printf(&quot;Can&#39;t find %s\n&quot;, f);
		exit(1);
	} else
		if (t == -1) {
			printf(&quot;Try again\n&quot;);
			return(1);
		}
	while(t!=wait(&amp;status));
	if ((t=(status&amp;0377)) != 0 &amp;&amp; t!=14) {
		if (t!=2)		/* interrupt */
			printf(&quot;Fatal error in %s\n&quot;, f);
		dexit();
	}
	return((status&gt;&gt;8) &amp; 0377);
}

copy(s)
char s[]; {
	char *otsp;

	otsp = tsp;
	while(*tsp++ = *s++);
	return(otsp);
}

nodup(l, s)
char **l, s[]; {
	char *t, *os, c;

	if (getsuf(s) != &#39;o&#39;)
		return(1);
	os = s;
	while(t = *l++) {
		s = os;
		while(c = *s++)
			if (c != *t++)
				break;
		if (*t++ == &#39;\0&#39;)
			return(0);
	}
	return(1);
}

cunlink(f)
char *f;
{
	if (f==0)
		return(0);
	return(unlink(f));
}
