#define	SBSIZE	2000
char	sbf[SBSIZE];
/* C command */

char *tmp0;
char *tmp1;
char *tmp2;
char *tmp3;
char *tmp4;
char *tmp5;
char ts[1000];
char *tsp ts;
char *av[50];
char *clist[50];
char *llist[50];
int instring;
int pflag;
int sflag;
int cflag;
int oflag;
int proflag;
int depth;
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
struct symtab *eifloc;
struct symtab *ifdloc;
struct symtab *ifnloc;
struct symtab *unxloc;
int	trulvl;
int	flslvl;
char *stringbuf;
char *pass0 &quot;/lib/c0&quot;;
char *pass1 &quot;/lib/c1&quot;;
char *pass2 &quot;/lib/c2&quot;;
char *pref &quot;/lib/crt0.o&quot;;

main(argc, argv)
char *argv[]; {
	char *t;
	int nc, nl, i, j, c, f20, nxo;
	int dexit();

	i = nc = nl = f20 = nxo = 0;
	while(++i &lt; argc) {
		if(*argv[i] == &#39;-&#39;)
			switch (argv[i][1]) {
				default:
					goto passa;
				case &#39;S&#39;:
					sflag++;
					cflag++;
					break;
				case &#39;O&#39;:
					oflag++;
					break;
				case &#39;p&#39;:
					proflag++;
					pref = &quot;/lib/mcrt0.o&quot;;
					break;
				case &#39;P&#39;:
					pflag++;
				case &#39;c&#39;:
					cflag++;
					break;

				case &#39;f&#39;:
					pref = &quot;/lib/fcrt0.o&quot;;
					pass0 = &quot;/lib/fc0&quot;;
					pass1 = &quot;/lib/fc1&quot;;
					break;

				case &#39;2&#39;:
					if(argv[i][2] == &#39;\0&#39;)
						pref = &quot;/lib/crt2.o&quot;;
					else {
						pref = &quot;/lib/crt20.o&quot;;
						f20 = 1;
					}
					break;
				case &#39;t&#39;:
					if (argv[i][2]==&#39;0&#39;)
						pass0 = &quot;/usr/c/c0&quot;;
					if (argv[i][2]==&#39;1&#39;)
						pass1 = &quot;/usr/c/c1&quot;;
					if (argv[i][2]==&#39;2&#39;)
						pass2 = &quot;/usr/c/c2&quot;;
					break;
			}
		else {
		passa:
			t = argv[i];
			if(getsuf(t)==&#39;c&#39;) {
				clist[nc++] = t;
				t = setsuf(t, &#39;o&#39;);
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
	if (pflag==0) {
		tmp0 = copy(&quot;/tmp/ctm0a&quot;);
		while((c=open(tmp0, 0))&gt;=0) {
			close(c);
			tmp0[9]++;
		}
		while((creat(tmp0, 0400))&lt;0)
			tmp0[9]++;
	}
	if ((signal(2, 1) &amp; 01) == 0)
		signal(2, &amp;dexit);
	(tmp1 = copy(tmp0))[8] = &#39;1&#39;;
	(tmp2 = copy(tmp0))[8] = &#39;2&#39;;
	(tmp3 = copy(tmp0))[8] = &#39;3&#39;;
	if (oflag)
		(tmp5 = copy(tmp0))[8] = &#39;5&#39;;
	if (pflag==0)
		(tmp4 = copy(tmp0))[8] = &#39;4&#39;;
	for (i=0; i&lt;nc; i++) {
		if (nc&gt;1)
			printf(&quot;%s:\n&quot;, clist[i]);
		av[0] = &quot;c0&quot;;
		if (pflag)
			tmp4 = setsuf(clist[i], &#39;i&#39;);
		av[1] = expand(clist[i]);
		if (pflag || exfail)
			continue;
		if (av[1] == 0) {
			cflag++;
			continue;
		}
		av[2] = tmp1;
		av[3] = tmp2;
		if (proflag) {
			av[4] = &quot;-P&quot;;
			av[5] = 0;
		} else
			av[4] = 0;
		if (callsys(pass0, av)) {
			cflag++;
			continue;
		}
		av[0] = &quot;c1&quot;;
		av[1] = tmp1;
		av[2] = tmp2;
		if (sflag)
			tmp3 = setsuf(clist[i], &#39;s&#39;);
		av[3] = tmp3;
		if (oflag)
			av[3] = tmp5;
		av[4] = 0;
		if(callsys(pass1, av)) {
			cflag++;
			continue;
		}
		if (oflag) {
			av[0] = &quot;c2&quot;;
			av[1] = tmp5;
			av[2] = tmp3;
			av[3] = 0;
			callsys(pass2, av);
			unlink(tmp5);
		}
		if (sflag)
			continue;
		av[0] = &quot;as&quot;;
		av[1] = &quot;-&quot;;
		av[2] = tmp3;
		av[3] = 0;
		cunlink(tmp1);
		cunlink(tmp2);
		cunlink(tmp4);
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
		av[1] = &quot;-X&quot;;
		av[2] = pref;
		j = 3;
		while(i&lt;nl)
			av[j++] = llist[i++];
		if(f20)
			av[j++] = &quot;-l2&quot;;
		else {
			av[j++] = &quot;-lc&quot;;
			av[j++] = &quot;-l&quot;;
		}
		av[j++] = 0;
		callsys(&quot;/bin/ld&quot;, av);
		if (nc==1 &amp;&amp; nxo==1)
			cunlink(setsuf(clist[0], &#39;o&#39;));
	}
	dexit();
}

dexit()
{
	if (!pflag) {
		cunlink(tmp1);
		cunlink(tmp2);
		if (sflag==0)
			cunlink(tmp3);
		cunlink(tmp4);
		cunlink(tmp5);
		cunlink(tmp0);
	}
	exit();
}

expand(file)
char *file;
{
	int ib1[259], ib2[259], ob[259];
	struct symtab stab[200];
	char ln[196];
	register int c;
	register char *rlp;

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
	insym(&amp;defloc, &quot;define&quot;);
	insym(&amp;incloc, &quot;include&quot;);
	insym(&amp;eifloc, &quot;endif&quot;);
	insym(&amp;ifdloc, &quot;ifdef&quot;);
	insym(&amp;ifnloc, &quot;ifndef&quot;);
	insym(&amp;unxloc, &quot;unix&quot;);
	stringbuf = sbf;
	trulvl = 0;
	flslvl = 0;
	line  = ln;
	lineno = 0;
	if (fcreat(tmp4, obuf) &lt; 0) {
		printf(&quot;Can&#39;t creat %s\n&quot;, tmp4);
		dexit();
	}
	while(getline()) {
		if (ibuf==ibuf2 &amp;&amp; pflag==0)
			putc(001, obuf);	/*SOH: insert */
		if (ln[0] != &#39;#&#39; &amp;&amp; flslvl==0)
			for (rlp = line; c = *rlp++;)
				putc(c, obuf);
		putc(&#39;\n&#39;, obuf);
	}
	for(rlp=line; c = *rlp++;)
			putc(c,obuf);
	fflush(obuf);
	close(obuf[0]);
	close(ibuf1[0]);
	return(tmp4);
}

getline()
{
	register int c, sc, state;
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
			if (state&gt;3) {
				if (flslvl==0 &amp;&amp;(state+!lookup(namep,-1)-&gt;name[0])==5)
					trulvl++;
				else
					flslvl++;
		out:
				while (c!=&#39;\n&#39; &amp;&amp; c!= &#39;\0&#39;)
					c = getch();
				return(c);
			}
			if (state!=2 || flslvl==0)
				{
				ungetc(c);
				np = lookup(namep, state);
				c = getch();
				}
			if (state==1) {
				if (np==defloc)
					state = 2;
				else if (np==incloc)
					state = 3;
				else if (np==ifnloc)
					state = 4;
				else if (np==ifdloc)
					state = 5;
				else if (np==eifloc) {
					if (flslvl)
						--flslvl;
					else if (trulvl)
						--trulvl;
					else error(&quot;If-less endif&quot;);
					goto out;
				}
				else {
					error(&quot;Undefined control&quot;);
					while (c!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;)
						c = getch();
					return(c);
				}
			} else if (state==2) {
				if (flslvl)
					goto out;
				np-&gt;value = stringbuf;
				savch(c);
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
			if (flslvl)
				goto out;
			if (state==3) {
				if (flslvl)
					goto out;
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

insym(sp, namep)
struct symtab **sp;
char *namep;
{
	register struct symtab *np;

	*sp = np = lookup(namep, 1);
	np-&gt;value = np-&gt;name;
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
	register char *rlp;

	rlp = lp;
	if (rlp==line+194)
		error(&quot;Line overflow&quot;);
	*rlp++ = c;
	if (rlp&gt;line+195)
		rlp = line+195;
	lp = rlp;
}

savch(c)
{
	*stringbuf++ = c;
	if (stringbuf-sbf &lt; SBSIZE)
		return;
	error(&quot;Too much defining&quot;);
	dexit();
}

getch()
{
	register int c;

loop:
	if ((c=getc1())==&#39;/&#39; &amp;&amp; !instring) {
		if ((c=getc1())!=&#39;*&#39;)
			{
			ungetc(c);
			return(&#39;/&#39;);
			}
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
char pushbuff[300];
char *pushp pushbuff;
ungetc(c)
	{
	*++pushp = c;
	}

getc1()
{
	register c;

	if (*pushp !=0)
		return(*pushp--);
	depth=0;
	if ((c = getc(ibuf)) &lt; 0 &amp;&amp; ibuf==ibuf2) {
		close(ibuf2[0]);
		ibuf = ibuf1;
		putc(&#39;\n&#39;, obuf);
		lineno++;
		c = getc1();
	}
	if (c&lt;0)
		return(0);
	return(c);
}

lookup(namep, enterf)
char *namep;
{
	register char *np, *snp;
	register struct symtab *sp;
	int i, c, around;
	np = namep;
	around = i = 0;
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
		if (++sp &gt;= &amp;symtab[symsiz])
			if (around++)
				{
				error(&quot;too many defines&quot;);
				dexit();
				}
			else
			sp = symtab;
	}
	if (enterf&gt;0) {
		snp = namep;
		for (np = &amp;sp-&gt;name[0]; np &lt; &amp;sp-&gt;name[8];)
			if (*np++ = *snp)
				snp++;
	}
	return(sp);
}
char revbuff[200];
char	*bp;
backsch(c)
	{
	if (bp-revbuff &gt; 200)
		error(&quot;Excessive define looping&quot;, bp--);
	*bp++ = c;
	}

subst(np, sp)
char *np;
struct symtab *sp;
{
	register char *vp;

	lp = np;
	bp = revbuff;
	if (depth++&gt;100)
		{
		error(&quot;define recursion loop\n&quot;);
		return;
		}
	if ((vp = sp-&gt;value) == 0)
		return;
	/* arrange that define unix unix still
	has no effect, avoiding rescanning */
	if (streq(sp-&gt;name,sp-&gt;value))
		{
		while (*vp)
			sch(*vp++);
		return;
		}
	backsch(&#39; &#39;);
	if (*vp == &#39;(&#39;)
		expdef(vp);
	else
	while (*vp)
		backsch(*vp++);
	backsch(&#39; &#39;);
	while (bp&gt;revbuff)
		ungetc(*--bp);
}

getsuf(as)
char as[];
{
	register int c;
	register char *s;
	register int t;

	s = as;
	c = 0;
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

setsuf(as, ch)
char as[];
{
	register char *s, *s1;

	s = s1 = copy(as);
	while(*s)
		if (*s++ == &#39;/&#39;)
			s1 = s;
	s[-1] = ch;
	return(s1);
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

copy(as)
char as[];
{
	register char *otsp, *s;

	otsp = tsp;
	s = as;
	while(*tsp++ = *s++);
	return(otsp);
}

nodup(l, os)
char **l, *os;
{
	register char *t, *s;
	register int c;

	s = os;
	if (getsuf(s) != &#39;o&#39;)
		return(1);
	while(t = *l++) {
		while(c = *s++)
			if (c != *t++)
				break;
		if (*t==&#39;\0&#39; &amp;&amp; c==&#39;\0&#39;)
			return(0);
		s = os;
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
expdef(proto)
  char *proto;
{
char buffer[100], *parg[20], *pval[20], name[20], *cspace, *wp;
char protcop[100], *pr;
int narg, k, i, c;
pr = protcop;
while (*pr++ = *proto++);
proto= protcop;
for (narg=0; (parg[narg] = token(&amp;proto)) != 0; narg++)
	;
/* now scan input */
cspace = buffer;
while ((c=getch()) == &#39; &#39;);
if (c != &#39;(&#39;)
	{
	error(&quot;defined function requires arguments&quot;);
	return;
	}
ungetc(c);
for(k=0; pval[k] = coptok(&amp;cspace); k++);
if (k!=narg)
 {
  error(&quot;define argument mismatch&quot;);
  return;
 }
while (c= *proto++)
   {
   if (!letter(c))
      backsch(c);
   else
      {
      wp = name;
      *wp++ = c;
      while (letnum(*proto))
        *wp++ = *proto++;
      *wp = 0;
      for (k=0; k&lt;narg; k++)
      if(streq(name,parg[k]))
        break;
      wp = k &lt;narg ? pval[k] : name;
      while (*wp) backsch(*wp++);
      }
   }
}
token(cpp) char **cpp;
{
char *val;
int stc;
stc = **cpp;
*(*cpp)++ = &#39;\0&#39;;
if (stc==&#39;)&#39;) return(0);
while (**cpp == &#39; &#39;) (*cpp)++;
for (val = *cpp; (stc= **cpp) != &#39;,&#39; &amp;&amp; stc!= &#39;)&#39;; (*cpp)++)
  {
  if (!letnum(stc) || (val == *cpp &amp;&amp; !letter(stc)))
    {
    error(&quot;define prototype argument error&quot;);
    break;
    }
  }
return(val);
}
coptok (cpp) char **cpp; {
char *val;
int stc, stop,paren;
paren = 0;
val = *cpp;
if (getch() == &#39;)&#39;)
  return(0);
while (((stc = getch()) != &#39;,&#39; &amp;&amp; stc != &#39;)&#39;) || paren &gt; 0)
  {
  if (stc == &#39;&quot;&#39; || stc == &#39;\&#39;&#39;)
    {
    stop = stc;
    if (stop == &#39;\&#39;&#39;)
      *(*cpp)++ = &#39;\&#39;&#39;;
    while ( (stc = getch()) != stop)
      {
      if (stc == &#39;\n&#39;)
        {
        error (&quot;non-terminated string&quot;);
        break;
        }
      if (stc == &#39;\\&#39;)
        if ((stc= getch()) != stop &amp;&amp; stc != &#39;\\&#39;)
          *(*cpp)++ = &#39;\\&#39;;
      *(*cpp)++ = stc;
      }
    if (stop == &#39;\&#39;&#39;) 
      *(*cpp)++ = &#39;\&#39;&#39;;
    }
  else if (stc == &#39;\\&#39;)
      {
      stc = getch();
      if (stc != &#39;&quot;&#39; &amp;&amp; stc != &#39;\\&#39;)
        *(*cpp)++ = &#39;\\&#39;;
      *(*cpp)++ = stc;
      }
  else
    {
    *(*cpp)++ = stc;
    if (stc == &#39;(&#39;)
        paren++;
    if (stc == &#39;)&#39;)
        paren--;
    }
  }
*(*cpp)++ = 0;
ungetc(stc);
return(val);
}
letter(c)
{
if ((c &gt;= &#39;a&#39; &amp;&amp; c &lt;= &#39;z&#39;) ||
    (c &gt;= &#39;A&#39; &amp;&amp; c &lt;= &#39;Z&#39;) ||
    (c == &#39;_&#39;))
    return (1);
else
    return(0);
}
letnum(c)
{
if (letter(c) || (c &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;))
  return(1);
else
  return(0);
}
streq(s,t) char *s, *t;
{
int c;
while ( (c= *s++) == *t++)
   if (c==0) return(1);
return(0);
}
