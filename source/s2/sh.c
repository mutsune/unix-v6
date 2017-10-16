#
/*
 */

#define	INTR	2
#define	QUIT	3
#define LINSIZ 1000
#define ARGSIZ 50
#define TRESIZ 100

#define QUOTE 0200
#define FAND 1
#define FCAT 2
#define FPIN 4
#define FPOU 8
#define FPAR 16
#define FINT 32
#define FPRS 64
#define TCOM 1
#define TPAR 2
#define TFIL 3
#define TLST 4
#define DTYP 0
#define DLEF 1
#define DRIT 2
#define DFLG 3
#define DSPR 4
#define DCOM 5
#define	ENOMEM	12
#define	ENOEXEC 8

char	*dolp;
char	pidp[6];
int	ldivr;
char	**dolv;
int	dolc;
char	*promp;
char	*linep;
char	*elinep;
char	**argp;
char	**eargp;
int	*treep;
int	*treeend;
char	peekc;
char	gflg;
char	error;
char	acctf;
char	uid;
char	setintr;
char	*arginp;
int	onelflg;

char	*mesg[] {
	0,
	&quot;Hangup&quot;,
	0,
	&quot;Quit&quot;,
	&quot;Illegal instruction&quot;,
	&quot;Trace/BPT trap&quot;,
	&quot;IOT trap&quot;,
	&quot;EMT trap&quot;,
	&quot;Floating exception&quot;,
	&quot;Killed&quot;,
	&quot;Bus error&quot;,
	&quot;Memory fault&quot;,
	&quot;Bad system call&quot;,
	0,
	&quot;Sig 14&quot;,
	&quot;Sig 15&quot;,
	&quot;Sig 16&quot;,
	&quot;Sig 17&quot;,
	&quot;Sig 18&quot;,
	&quot;Sig 19&quot;,
};

struct stime {
	int proct[2];
	int cputim[2];
	int systim[2];
} timeb;

char	line[LINSIZ];
char	*args[ARGSIZ];
int	trebuf[TRESIZ];

main(c, av)
int c;
char **av;
{
	register f;
	register char *acname, **v;

	for(f=2; f&lt;15; f++)
		close(f);
	if((f=dup(1)) != 2)
		close(f);
	dolc = getpid();
	for(f=4; f&gt;=0; f--) {
		dolc = ldiv(0, dolc, 10);
		pidp[f] = ldivr+&#39;0&#39;;
	}
	v = av;
	acname = &quot;/usr/adm/sha&quot;;
	promp = &quot;% &quot;;
	if(((uid = getuid())&amp;0377) == 0)
		promp = &quot;# &quot;;
	acctf = open(acname, 1);
	if(c &gt; 1) {
		promp = 0;
		if (*v[1]==&#39;-&#39;) {
			**v = &#39;-&#39;;
			if (v[1][1]==&#39;c&#39; &amp;&amp; c&gt;2)
				arginp = v[2];
			else if (v[1][1]==&#39;t&#39;)
				onelflg = 2;
		} else {
			close(0);
			f = open(v[1], 0);
			if(f &lt; 0) {
				prs(v[1]);
				err(&quot;: cannot open&quot;);
			}
		}
	}
	if(**v == &#39;-&#39;) {
		setintr++;
		signal(QUIT, 1);
		signal(INTR, 1);
	}
	dolv = v+1;
	dolc = c-1;

loop:
	if(promp != 0)
		prs(promp);
	peekc = getc();
	main1();
	goto loop;
}

main1()
{
	register char c, *cp;
	register *t;

	argp = args;
	eargp = args+ARGSIZ-5;
	linep = line;
	elinep = line+LINSIZ-5;
	error = 0;
	gflg = 0;
	do {
		cp = linep;
		word();
	} while(*cp != &#39;\n&#39;);
	treep = trebuf;
	treeend = &amp;trebuf[TRESIZ];
	if(gflg == 0) {
		if(error == 0) {
			setexit();
			if (error)
				return;
			t = syntax(args, argp);
		}
		if(error != 0)
			err(&quot;syntax error&quot;); else
			execute(t);
	}
}

word()
{
	register char c, c1;

	*argp++ = linep;

loop:
	switch(c = getc()) {

	case &#39; &#39;:
	case &#39;\t&#39;:
		goto loop;

	case &#39;\&#39;&#39;:
	case &#39;&quot;&#39;:
		c1 = c;
		while((c=readc()) != c1) {
			if(c == &#39;\n&#39;) {
				error++;
				peekc = c;
				return;
			}
			*linep++ = c|QUOTE;
		}
		goto pack;

	case &#39;&amp;&#39;:
	case &#39;;&#39;:
	case &#39;&lt;&#39;:
	case &#39;&gt;&#39;:
	case &#39;(&#39;:
	case &#39;)&#39;:
	case &#39;|&#39;:
	case &#39;^&#39;:
	case &#39;\n&#39;:
		*linep++ = c;
		*linep++ = &#39;\0&#39;;
		return;
	}

	peekc = c;

pack:
	for(;;) {
		c = getc();
		if(any(c, &quot; &#39;\&quot;\t;&amp;&lt;&gt;()|^\n&quot;)) {
			peekc = c;
			if(any(c, &quot;\&quot;&#39;&quot;))
				goto loop;
			*linep++ = &#39;\0&#39;;
			return;
		}
		*linep++ = c;
	}
}

tree(n)
int n;
{
	register *t;

	t = treep;
	treep =+ n;
	if (treep&gt;treeend) {
		prs(&quot;Command line overflow\n&quot;);
		error++;
		reset();
	}
	return(t);
}

getc()
{
	register char c;

	if(peekc) {
		c = peekc;
		peekc = 0;
		return(c);
	}
	if(argp &gt; eargp) {
		argp =- 10;
		while((c=getc()) != &#39;\n&#39;);
		argp =+ 10;
		err(&quot;Too many args&quot;);
		gflg++;
		return(c);
	}
	if(linep &gt; elinep) {
		linep =- 10;
		while((c=getc()) != &#39;\n&#39;);
		linep =+ 10;
		err(&quot;Too many characters&quot;);
		gflg++;
		return(c);
	}
getd:
	if(dolp) {
		c = *dolp++;
		if(c != &#39;\0&#39;)
			return(c);
		dolp = 0;
	}
	c = readc();
	if(c == &#39;\\&#39;) {
		c = readc();
		if(c == &#39;\n&#39;)
			return(&#39; &#39;);
		return(c|QUOTE);
	}
	if(c == &#39;$&#39;) {
		c = readc();
		if(c&gt;=&#39;0&#39; &amp;&amp; c&lt;=&#39;9&#39;) {
			if(c-&#39;0&#39; &lt; dolc)
				dolp = dolv[c-&#39;0&#39;];
			goto getd;
		}
		if(c == &#39;$&#39;) {
			dolp = pidp;
			goto getd;
		}
	}
	return(c&amp;0177);
}

readc()
{
	char cc;
	register c;

	if (arginp) {
		if (arginp == 1)
			exit();
		if ((c = *arginp++) == 0) {
			arginp = 1;
			c = &#39;\n&#39;;
		}
		return(c);
	}
	if (onelflg==1)
		exit();
	if(read(0, &amp;cc, 1) != 1)
		exit();
	if (cc==&#39;\n&#39; &amp;&amp; onelflg)
		onelflg--;
	return(cc);
}

/*
 * syntax
 *	empty
 *	syn1
 */

syntax(p1, p2)
char **p1, **p2;
{

	while(p1 != p2) {
		if(any(**p1, &quot;;&amp;\n&quot;))
			p1++; else
			return(syn1(p1, p2));
	}
	return(0);
}

/*
 * syn1
 *	syn2
 *	syn2 &amp; syntax
 *	syn2 ; syntax
 */

syn1(p1, p2)
char **p1, **p2;
{
	register char **p;
	register *t, *t1;
	int l;

	l = 0;
	for(p=p1; p!=p2; p++)
	switch(**p) {

	case &#39;(&#39;:
		l++;
		continue;

	case &#39;)&#39;:
		l--;
		if(l &lt; 0)
			error++;
		continue;

	case &#39;&amp;&#39;:
	case &#39;;&#39;:
	case &#39;\n&#39;:
		if(l == 0) {
			l = **p;
			t = tree(4);
			t[DTYP] = TLST;
			t[DLEF] = syn2(p1, p);
			t[DFLG] = 0;
			if(l == &#39;&amp;&#39;) {
				t1 = t[DLEF];
				t1[DFLG] =| FAND|FPRS|FINT;
			}
			t[DRIT] = syntax(p+1, p2);
			return(t);
		}
	}
	if(l == 0)
		return(syn2(p1, p2));
	error++;
}

/*
 * syn2
 *	syn3
 *	syn3 | syn2
 */

syn2(p1, p2)
char **p1, **p2;
{
	register char **p;
	register int l, *t;

	l = 0;
	for(p=p1; p!=p2; p++)
	switch(**p) {

	case &#39;(&#39;:
		l++;
		continue;

	case &#39;)&#39;:
		l--;
		continue;

	case &#39;|&#39;:
	case &#39;^&#39;:
		if(l == 0) {
			t = tree(4);
			t[DTYP] = TFIL;
			t[DLEF] = syn3(p1, p);
			t[DRIT] = syn2(p+1, p2);
			t[DFLG] = 0;
			return(t);
		}
	}
	return(syn3(p1, p2));
}

/*
 * syn3
 *	( syn1 ) [ &lt; in  ] [ &gt; out ]
 *	word word* [ &lt; in ] [ &gt; out ]
 */

syn3(p1, p2)
char **p1, **p2;
{
	register char **p;
	char **lp, **rp;
	register *t;
	int n, l, i, o, c, flg;

	flg = 0;
	if(**p2 == &#39;)&#39;)
		flg =| FPAR;
	lp = 0;
	rp = 0;
	i = 0;
	o = 0;
	n = 0;
	l = 0;
	for(p=p1; p!=p2; p++)
	switch(c = **p) {

	case &#39;(&#39;:
		if(l == 0) {
			if(lp != 0)
				error++;
			lp = p+1;
		}
		l++;
		continue;

	case &#39;)&#39;:
		l--;
		if(l == 0)
			rp = p;
		continue;

	case &#39;&gt;&#39;:
		p++;
		if(p!=p2 &amp;&amp; **p==&#39;&gt;&#39;)
			flg =| FCAT; else
			p--;

	case &#39;&lt;&#39;:
		if(l == 0) {
			p++;
			if(p == p2) {
				error++;
				p--;
			}
			if(any(**p, &quot;&lt;&gt;(&quot;))
				error++;
			if(c == &#39;&lt;&#39;) {
				if(i != 0)
					error++;
				i = *p;
				continue;
			}
			if(o != 0)
				error++;
			o = *p;
		}
		continue;

	default:
		if(l == 0)
			p1[n++] = *p;
	}
	if(lp != 0) {
		if(n != 0)
			error++;
		t = tree(5);
		t[DTYP] = TPAR;
		t[DSPR] = syn1(lp, rp);
		goto out;
	}
	if(n == 0)
		error++;
	p1[n++] = 0;
	t = tree(n+5);
	t[DTYP] = TCOM;
	for(l=0; l&lt;n; l++)
		t[l+DCOM] = p1[l];
out:
	t[DFLG] = flg;
	t[DLEF] = i;
	t[DRIT] = o;
	return(t);
}

scan(at, f)
int *at;
int (*f)();
{
	register char *p, c;
	register *t;

	t = at+DCOM;
	while(p = *t++)
		while(c = *p)
			*p++ = (*f)(c);
}

tglob(c)
int c;
{

	if(any(c, &quot;[?*&quot;))
		gflg = 1;
	return(c);
}

trim(c)
int c;
{

	return(c&amp;0177);
}

execute(t, pf1, pf2)
int *t, *pf1, *pf2;
{
	int i, f, pv[2];
	register *t1;
	register char *cp1, *cp2;
	extern errno;

	if(t != 0)
	switch(t[DTYP]) {

	case TCOM:
		cp1 = t[DCOM];
		if(equal(cp1, &quot;chdir&quot;)) {
			if(t[DCOM+1] != 0) {
				if(chdir(t[DCOM+1]) &lt; 0)
					err(&quot;chdir: bad directory&quot;);
			} else
				err(&quot;chdir: arg count&quot;);
			return;
		}
		if(equal(cp1, &quot;shift&quot;)) {
			if(dolc &lt; 1) {
				prs(&quot;shift: no args\n&quot;);
				return;
			}
			dolv[1] = dolv[0];
			dolv++;
			dolc--;
			return;
		}
		if(equal(cp1, &quot;login&quot;)) {
			if(promp != 0) {
				close(acctf);
				execv(&quot;/bin/login&quot;, t+DCOM);
			}
			prs(&quot;login: cannot execute\n&quot;);
			return;
		}
		if(equal(cp1, &quot;newgrp&quot;)) {
			if(promp != 0) {
				close(acctf);
				execv(&quot;/bin/newgrp&quot;, t+DCOM);
			}
			prs(&quot;newgrp: cannot execute\n&quot;);
			return;
		}
		if(equal(cp1, &quot;wait&quot;)) {
			pwait(-1, 0);
			return;
		}
		if(equal(cp1, &quot;:&quot;))
			return;

	case TPAR:
		f = t[DFLG];
		i = 0;
		if((f&amp;FPAR) == 0)
			i = fork();
		if(i == -1) {
			err(&quot;try again&quot;);
			return;
		}
		if(i != 0) {
			if((f&amp;FPIN) != 0) {
				close(pf1[0]);
				close(pf1[1]);
			}
			if((f&amp;FPRS) != 0) {
				prn(i);
				prs(&quot;\n&quot;);
			}
			if((f&amp;FAND) != 0)
				return;
			if((f&amp;FPOU) == 0)
				pwait(i, t);
			return;
		}
		if(t[DLEF] != 0) {
			close(0);
			i = open(t[DLEF], 0);
			if(i &lt; 0) {
				prs(t[DLEF]);
				err(&quot;: cannot open&quot;);
				exit();
			}
		}
		if(t[DRIT] != 0) {
			if((f&amp;FCAT) != 0) {
				i = open(t[DRIT], 1);
				if(i &gt;= 0) {
					seek(i, 0, 2);
					goto f1;
				}
			}
			i = creat(t[DRIT], 0666);
			if(i &lt; 0) {
				prs(t[DRIT]);
				err(&quot;: cannot create&quot;);
				exit();
			}
		f1:
			close(1);
			dup(i);
			close(i);
		}
		if((f&amp;FPIN) != 0) {
			close(0);
			dup(pf1[0]);
			close(pf1[0]);
			close(pf1[1]);
		}
		if((f&amp;FPOU) != 0) {
			close(1);
			dup(pf2[1]);
			close(pf2[0]);
			close(pf2[1]);
		}
		if((f&amp;FINT)!=0 &amp;&amp; t[DLEF]==0 &amp;&amp; (f&amp;FPIN)==0) {
			close(0);
			open(&quot;/dev/null&quot;, 0);
		}
		if((f&amp;FINT) == 0 &amp;&amp; setintr) {
			signal(INTR, 0);
			signal(QUIT, 0);
		}
		if(t[DTYP] == TPAR) {
			if(t1 = t[DSPR])
				t1[DFLG] =| f&amp;FINT;
			execute(t1);
			exit();
		}
		close(acctf);
		gflg = 0;
		scan(t, &amp;tglob);
		if(gflg) {
			t[DSPR] = &quot;/etc/glob&quot;;
			execv(t[DSPR], t+DSPR);
			prs(&quot;glob: cannot execute\n&quot;);
			exit();
		}
		scan(t, &amp;trim);
		*linep = 0;
		texec(t[DCOM], t);
		cp1 = linep;
		cp2 = &quot;/usr/bin/&quot;;
		while(*cp1 = *cp2++)
			cp1++;
		cp2 = t[DCOM];
		while(*cp1++ = *cp2++);
		texec(linep+4, t);
		texec(linep, t);
		prs(t[DCOM]);
		err(&quot;: not found&quot;);
		exit();

	case TFIL:
		f = t[DFLG];
		pipe(pv);
		t1 = t[DLEF];
		t1[DFLG] =| FPOU | (f&amp;(FPIN|FINT|FPRS));
		execute(t1, pf1, pv);
		t1 = t[DRIT];
		t1[DFLG] =| FPIN | (f&amp;(FPOU|FINT|FAND|FPRS));
		execute(t1, pv, pf2);
		return;

	case TLST:
		f = t[DFLG]&amp;FINT;
		if(t1 = t[DLEF])
			t1[DFLG] =| f;
		execute(t1);
		if(t1 = t[DRIT])
			t1[DFLG] =| f;
		execute(t1);
		return;

	}
}

texec(f, at)
int *at;
{
	extern errno;
	register int *t;

	t = at;
	execv(f, t+DCOM);
	if (errno==ENOEXEC) {
		if (*linep)
			t[DCOM] = linep;
		t[DSPR] = &quot;/bin/sh&quot;;
		execv(t[DSPR], t+DSPR);
		prs(&quot;No shell!\n&quot;);
		exit();
	}
	if (errno==ENOMEM) {
		prs(t[DCOM]);
		err(&quot;: too large&quot;);
		exit();
	}
}

err(s)
char *s;
{

	prs(s);
	prs(&quot;\n&quot;);
	if(promp == 0) {
		seek(0, 0, 2);
		exit();
	}
}

prs(as)
char *as;
{
	register char *s;

	s = as;
	while(*s)
		putc(*s++);
}

putc(c)
{

	write(2, &amp;c, 1);
}

prn(n)
int n;
{
	register a;

	if(a=ldiv(0,n,10))
		prn(a);
	putc(lrem(0,n,10)+&#39;0&#39;);
}

any(c, as)
int c;
char *as;
{
	register char *s;

	s = as;
	while(*s)
		if(*s++ == c)
			return(1);
	return(0);
}

equal(as1, as2)
char *as1, *as2;
{
	register char *s1, *s2;

	s1 = as1;
	s2 = as2;
	while(*s1++ == *s2)
		if(*s2++ == &#39;\0&#39;)
			return(1);
	return(0);
}

pwait(i, t)
int i, *t;
{
	register p, e;
	int s;

	if(i != 0)
	for(;;) {
		times(&amp;timeb);
		time(timeb.proct);
		p = wait(&amp;s);
		if(p == -1)
			break;
		e = s&amp;0177;
		if(mesg[e] != 0) {
			if(p != i) {
				prn(p);
				prs(&quot;: &quot;);
			}
			prs(mesg[e]);
			if(s&amp;0200)
				prs(&quot; -- Core dumped&quot;);
		}
		if(e != 0)
			err(&quot;&quot;);
		if(i == p) {
			acct(t);
			break;
		} else
			acct(0);
	}
}

acct(t)
int *t;
{
	if(t == 0)
		enacct(&quot;**gok&quot;); else
	if(*t == TPAR)
		enacct(&quot;()&quot;); else
	enacct(t[DCOM]);
}

enacct(as)
char *as;
{
	struct stime timbuf;
	struct {
		char cname[14];
		char shf;
		char uid;
		int datet[2];
		int realt[2];
		int bcput[2];
		int bsyst[2];
	} tbuf;
	register i;
	register char *np, *s;

	s = as;
	times(&amp;timbuf);
	time(timbuf.proct);
	lsub(tbuf.realt, timbuf.proct, timeb.proct);
	lsub(tbuf.bcput, timbuf.cputim, timeb.cputim);
	lsub(tbuf.bsyst, timbuf.systim, timeb.systim);
	do {
		np = s;
		while (*s != &#39;\0&#39; &amp;&amp; *s != &#39;/&#39;)
			s++;
	} while (*s++ != &#39;\0&#39;);
	for (i=0; i&lt;14; i++) {
		tbuf.cname[i] = *np;
		if (*np)
			np++;
	}
	tbuf.datet[0] = timbuf.proct[0];
	tbuf.datet[1] = timbuf.proct[1];
	tbuf.uid = uid;
	tbuf.shf = 0;
	if (promp==0)
		tbuf.shf = 1;
	seek(acctf, 0, 2);
	write(acctf, &amp;tbuf, sizeof(tbuf));
}
