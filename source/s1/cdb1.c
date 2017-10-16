#
/*

	C debugger

*/

#include &quot;/usr/sys/param.h&quot;
#include &quot;/usr/sys/user.h&quot;

#define	DSP	0
#define	ISP	1
#define	NBKP	10
#define	SYMSIZ	12*400
#define	BADJST	01

int	fcore;
int	errno;
int	fsym;
int	symoff;
char	*lp;
int	errflg;
int	symlen;
int	symct;
int	symcor;
int	symbuf[SYMSIZ];
int	*symptr;
struct {
	int	loc;
	int	ins;
	int	count;
	int	flag;
} bkptl[NBKP];
int	lastbp;
char	symbol[8];
int	symflg;
int	symval;
char	tsym[8];
char	fsymbol[10];
char	ssymbol[8];
int	ssymflg;
int	ssymval;
int	signo;
char	line[128];
int	regbuf[512];
char	**uregs	&amp;regbuf[512];
char	*rtsize;
int	loccsv;
int	locsr5;
#define	RUSER	1
#define	RIUSER	2
#define	WUSER	4
#define	RUREGS	3
#define	WUREGS	6
#define	SETTRC	0
#define	CONTIN	7
#define	EXIT	8

#define	ps	-1
#define	pc	-2
#define	sp	-6
#define	r5	-9
#define	r4	-10
#define	r3	-11
#define	r2	-12
#define	r1	-5
#define	r0	-3
struct reglist {
	char	*rname;
	int	roffs;
} reglist[] {
	&quot;ps&quot;, ps,
	&quot;pc&quot;, pc,
	&quot;sp&quot;, sp,
	&quot;r5&quot;, r5,
	&quot;r4&quot;, r4,
	&quot;r3&quot;, r3,
	&quot;r2&quot;, r2,
	&quot;r1&quot;, r1,
	&quot;r0&quot;, r0,
};

struct sfregs {
	int	junk[2];
	int	fpsr;
	float	sfr[6];
};

struct lfregs {
	int	junk[2];
	int	fpsr;
	double	lfr[6];
};

int	frnames[] { 0, 3, 4, 5, 1, 2 };
int	dot;
int	tdot;
int	dotinc 2;
int	lastcom &#39;/&#39;;
int	lastype	&#39;o&#39;;
int	modifier;
char	*symfil	&quot;a.out&quot;;
char	*corfil	&quot;core&quot;;
int	callist[50];
int	entpt[50];
int	callev;
int	pid;
int	adrflg;
int	idsep;
char	*signals[] {
	&quot;&quot;,
	&quot;Hangup&quot;,
	&quot;Interrupt&quot;,
	&quot;Quit&quot;,
	&quot;Illegal instruction&quot;,
	&quot;Trace/BTP&quot;,
	&quot;IOT&quot;,
	&quot;EMT&quot;,
	&quot;Floating exception&quot;,
	&quot;Killed&quot;,
	&quot;Bus error&quot;,
	&quot;Memory fault&quot;,
	&quot;Bad system call&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,
};

main(argc, argv)
char **argv;
{
	int onintr();

	if (argc&gt;1)
		symfil = argv[1];
	if (argc&gt;2)
		corfil = argv[2];
	fcore = open(corfil, 0);
	if ((fsym = open(symfil, 0)) &lt; 0) {
		printf(&quot;%s not found\n&quot;, symfil);
		return;
	}
	read(fsym, regbuf, 020);
	if (regbuf[0]==0411)			/* I/D separated */
		idsep++;
	else if (regbuf[0]!=0410 &amp;&amp; regbuf[0]!=0407) {	/* magic */
		printf(&quot;Bad format: %s\n&quot;, symfil);
		return;
	}
	symoff = regbuf[1] + regbuf[2];
	symlen = regbuf[4];
	if (regbuf[7] != 1)
		symoff =&lt;&lt; 1;
	symoff =+ 020;
	seek(fsym, symoff, 0);
	symcor = read(fsym, symbuf, sizeof symbuf);
	if (symcor&gt;0)
		symoff =+ symcor;
	symcor =&gt;&gt; 1;
	read(fcore, regbuf, 1024);
	signo = regbuf[0].u_arg[0]&amp;017;
	regbuf-&gt;u_tsize =&lt;&lt; 6;
	regbuf-&gt;u_dsize =&lt;&lt; 6;
	regbuf-&gt;u_ssize =&lt;&lt; 6;
	rtsize = (regbuf-&gt;u_tsize+017777) &amp; ~017777;
	if (symlook(&quot;csv\0\0\0\0&quot;))
		loccsv = ssymval;
	if (symlook(&quot;savr5\0\0\0&quot;))
		locsr5 = ssymval;
	setstack();
	signal(SIGINS, onintr);
	setexit();
	signal(SIGINT, onintr);
loop:
	if (errflg) {
		printf(&quot;?\n&quot;);
		errflg = 0;
	}
	lp = line;
	while ((*lp = getchar()) != &#39;\n&#39;)
		if (*lp++ == &#39;\0&#39;) {
			if (pid)
				ptrace(EXIT, pid, 0, 0);
			return;
		}
	lp = line;
	command();
	goto loop;
}

command()
{
	register n;

	adrflg = expr();
	if (errflg)
		return;
	n = getcnt();
	if (lastcom==&#39;$&#39;)
		lastcom = &#39;/&#39;;
	if (*lp == &#39;\n&#39;) {
		if (!adrflg)
			dot =+ dotinc;
	} else
		lastcom = *lp++;
	modifier = 0;
	if (*lp != &#39;\n&#39;)
		modifier = *lp++;
	if (lastcom==&#39;%&#39; &amp;&amp; modifier==&#39;r&#39;) {
		runcom();
		return;
	}
	if (*lp != &#39;\n&#39;) {
		errflg++;
		return;
	}
	if (adrflg)
		dot = tdot;
	while(n) {
		scommand(n);
		if (errflg)
			return;
		if (--n)
			dot =+ dotinc;
	}
}

scommand(n)
{
	register w, c;
	double fw;
	struct { int i[4]; };
	int onintr();

	switch(lastcom) {

	case &#39;/&#39;:
		w = cget(dot, DSP);
		if (modifier)
			lastype = modifier;
		switch(lastype) {

		case &#39;o&#39;:
			printf(&quot;%.1o\n&quot;, w);
			dotinc = 2;
			return;

		case &#39;i&#39;:
			printf(&quot;%d\n&quot;, w);
			dotinc = 2;
			return;

		case &#39;f&#39;:
			fw = 0;
			fw.i[0] = w;
			fw.i[1] = cget(dot+2, DSP);
			printf(&quot;%e\n&quot;, fw);
			dotinc = 4;
			return;

		case &#39;d&#39;:
			fw.i[0] = w;
			fw.i[1] = cget(dot+2, DSP);
			fw.i[2] = cget(dot+4, DSP);
			fw.i[3] = cget(dot+6, DSP);
			printf(&quot;%e\n&quot;, fw);
			dotinc = 8;
			return;
		}
		errflg++;
		return;

	case &#39;\\&#39;:
		printf(&quot;%.1o\n&quot;, cget(dot, DSP)&amp;0377);
		dotinc = 1;
		return;

	case &#39;=&#39;:
		printf(&quot;%.1o\n&quot;, dot);
		return;

	case &#39;\&#39;&#39;:
		printc(cget(dot, DSP) &amp; 0377);
		if (n&lt;=1)
			putchar(&#39;\n&#39;);
		dotinc = 1;
		return;

	case &#39;&quot;&#39;:
		w = cget(dot, DSP);
		while(c = cget(w++, DSP)&amp;0377)
			printc(c);
		putchar(&#39;\n&#39;);
		return;

	case &#39;&amp;&#39;:
		psymoff(cget(dot, DSP), 0100000);
		printf(&quot;\n&quot;);
		return;

	case &#39;$&#39;:
		printf(&quot;%s\n&quot;, signals[signo]);
		printtrace();
		return;

	case &#39;?&#39;:
		printins(0);
		printf(&quot;\n&quot;);
		return;

	case &#39;%&#39;:
		runcom();
		signal(SIGINT, onintr);
		return;

	}
	errflg++;
}

getcnt()
{
	register t1, t2;

	if (*lp != &#39;,&#39;)
		return(1);
	lp++;
	t1 = tdot;
	if (expr() == 0) {
		tdot = t1;
		return(1);
	}
	t2 = tdot;
	tdot = t1;
	return(t2);
}

cget(n, space)
{
	register w;

	w = get(n, space);
	if (errflg)
		reset();
	return(w);
}

printc(c)
{
	if (c&lt;&#39; &#39; || c&gt;&#39;~&#39;)
		printf(&quot;\\%o&quot;, c);
	else
		printf(&quot;%c&quot;, c);
}

expr()
{
	int i, t1, t2, donef, lastop, b;

	tdot = 0;
	adrflg = 0;
	lastop = &#39;+&#39;;
	ssymval = 0;
	donef = 0;
loop:
	fsymbol[0] = 0;
	if (symchar(0)) {
		adrflg++;
		symcollect(&#39;_&#39;);
		if (*lp++==&#39;:&#39; &amp;&amp; symchar(0)) {
			for (i=0; i&lt;8; i++)
				fsymbol[i] = tsym[i];
			fsymbol[0] = &#39;~&#39;;
			symcollect(0);
		} else 
			lp--;
		if (symlook(tsym) == 0) {
			errflg++;
			reset();
		}
		goto loop;
	}
	if (*lp&gt;=&#39;0&#39; &amp;&amp; *lp&lt;=&#39;9&#39;) {
		adrflg++;
		ssymval = 0;
		if (*lp == &#39;0&#39;)
			b = 8;
		else
			b = 10;
		while (*lp&gt;=&#39;0&#39; &amp;&amp; *lp&lt;=&#39;9&#39;) {
			ssymval =* b;
			ssymval =+ *lp++ -&#39;0&#39;;
		}
		goto loop;
	}
	switch (*lp) {

	default:
		donef++;

	case &#39;+&#39;:
	case &#39;-&#39;:
		switch(lastop) {

		case &#39;+&#39;:
			tdot =+ ssymval;
			goto op;

		case &#39;-&#39;:
			tdot =- ssymval;

		op:
			if (donef)
				return(adrflg);
			else
				lastop = *lp++;
		}
		goto loop;

	case &#39; &#39;:
	case &#39;\t&#39;:
		lp++;
		goto loop;

	case &#39;[&#39;:
		lp++;
		t1 = ssymval;
		t2 = tdot;
		if (expr() == 0)
			tdot = 0;
		ssymval = cget(t1 + (tdot&lt;&lt;1), DSP);
		tdot = t2;
		if (*lp == &#39;]&#39;)
			lp++;
		goto loop;
	}
}

symcollect(c)
{
	register char *p;

	p = tsym;
	if (c)
		*p++ = c;
	while (symchar(1)) {
		if (p &lt; &amp;tsym[8])
			*p++ = *lp;
		lp++;
	}
	while (p &lt; &amp;tsym[8])
		*p++ = 0;
}

symchar(dig)
{
	if (*lp&gt;=&#39;a&#39;&amp;&amp;*lp&lt;=&#39;z&#39; || *lp==&#39;_&#39;)
		return(1);
	if (dig &amp;&amp; *lp&gt;=&#39;0&#39; &amp;&amp; *lp&lt;=&#39;9&#39;)
		return(1);
	return(0);
}

error()
{
	errflg++;
	reset();
}

printtrace()
{
	int tpc, tr5, narg, argp, i;

	if (modifier==&#39;r&#39;) {
		printregs();
		return;
	}
	if (modifier==&#39;f&#39; || modifier==&#39;d&#39;) {
		printfregs();
		return;
	}
	tpc = uregs[pc];
	tr5 = uregs[r5];
	if (locsr5)
		if (narg = get(locsr5, DSP)) {
			tr5 = narg;
		}
	callev = 0;
	while (errflg == 0) {
		narg = findroutine(tpc, tr5);
		printf(&quot;%2d: %.8s(&quot;, callev, ssymbol);
		if (--narg &gt;= 0)
			printf(&quot;%.1o&quot;, get(tr5+4, DSP));
		argp = tr5+4;
		while(--narg &gt;= 0)
			printf(&quot;,%.1o&quot;, get(argp =+ 2, DSP));
		printf(&quot;)\n&quot;);
		tpc = get(tr5+2, DSP);
		if (callev &lt; 50) {
			entpt[callev] = ssymval;
			callist[callev++] = tr5;
		}
		if ((tr5 = get(tr5, DSP)) == 0)
			break;
	}
}

setstack()
{
	register int tpc, tr5, i;

	tpc = uregs[pc];
	tr5 = uregs[r5];
	if (locsr5)
		if (i = get(locsr5, DSP)) {
			tr5 = i;
		}
	callev = 0;
	while (errflg == 0) {
		findroutine(tpc, tr5);
		tpc = get(tr5+2, DSP);
		if (callev &gt;= 50)
			break;
		entpt[callev] = ssymval;
		callist[callev++] = tr5;
		if ((tr5 = get(tr5, DSP)) == 0)
			break;
	}
	errflg = 0;
}

printfregs()
{
	register i;
	double f;

	printf(&quot;fpsr	%.1o\n&quot;, regbuf[0].fpsr);
	for (i=0; i&lt;6; i++) {
		if (regbuf[0].fpsr&amp;0200)	/* long mode */
			f = regbuf[0].lfr[frnames[i]];
		else
			f = regbuf[0].sfr[frnames[i]];
		printf(&quot;fr%d	%e\n&quot;, i, f);
	}
}

printregs()
{
	register struct reglist *p;
	register char *v, *d;

	for (p=reglist; p &lt; &amp;reglist[9]; p++) {
		printf(&quot;%s	%.1o&quot;, p-&gt;rname, v=uregs[p-&gt;roffs]);
		d = vallook(v);
		if (d &lt; 010000) {
			printf(&quot;	%.8s&quot;, ssymbol);
			if (d)
				printf(&quot;+%.1o&quot;, d);
		}
		printf(&quot;\n&quot;);
	}
}

findroutine(rpc, rr5)
{
	register callpt, inst, narg;

	callpt = get(rr5+2, DSP);
	if ((inst=get(callpt-4, ISP)) == 04737)	/* jsr pc,*$... */
		narg = 1;
	else if ((inst&amp;~077)==04700)		/* jsr pc,... */
		narg = 0;
	else {
		errflg++;
		return(0);
	}
	inst = vallook((inst==04767?callpt:0) + get(callpt-2, ISP));
	if (inst) {
		ssymbol[0] = &#39;?&#39;;
		ssymbol[1] = 0;
		ssymval = 0;
	}
	inst = get(callpt, ISP);
	if (inst == 05726)		/* tst (sp)+ */
		return(narg+1);
	if (inst == 022626)		/* cmp (sp)+,(sp)+ */
		return(narg+2);
	if (inst == 062706)		/* add $n,sp */
		return(narg+get(callpt+2, ISP)/2);
	return(narg);
}

runcom()
{
	int stat;
	register w, i;

	switch(modifier) {


	/* delete breakpoint */
	case &#39;d&#39;:
		if (adrflg==0)
			error();
		for (w=0; w&lt;NBKP; w++) {
			i = bkptl[w].loc;
			if (bkptl[w].flag &amp; BADJST)
				i =- 4;
			if (dot==i) {
				if (lastbp==bkptl[w].loc) {
					ptrace(WUREGS,pid,2*(512+ps),uregs[ps]&amp;~020);
					lastbp = 0;
				} else {
					ptrace(WUSER,pid,bkptl[w].loc,bkptl[w].ins);
				}
				bkptl[w].loc = 0;
				bkptl[w].flag = 0;
				return;
			}
		}
		error();

	/* set breakpoint */
	case &#39;b&#39;:
		if (adrflg==0)
			error();
		for (w=0; w&lt;NBKP; w++) {
			i = bkptl[w].loc;
			if (bkptl[w].flag&amp;BADJST)
				i =- 4;
			if (i==dot)
				return;
		}
		for (w=0; w&lt;NBKP; w++)
			if (bkptl[w].loc==0) {
				bkptl[w].loc = dot;
				return;
			}
		error();

	/* run program */
	case &#39;r&#39;:
		lastbp = 0;
		if (pid) {
			ptrace(EXIT, pid, 0, 0);
			pid = 0;
		}
		if ((pid = fork())==0) {
			ptrace(SETTRC, 0, 0, 0);
			signal(SIGINT, 0);
			signal(SIGINS, 0);
			doexec();
			printf(&quot;Can&#39;t execute %s\n&quot;, symfil);
			exit(0);
		}
		bpwait(0);
		setbp(1);
		ptrace(WUREGS, pid, 2*(512+ps), 0170000);

	case &#39;c&#39;:
		if (pid==0)
			error();
		setbp(0);
		if (lastbp) {
			w = lastbp;
			ptrace(CONTIN, pid, 0, 0);
			bpwait(1);
			ptrace(WUSER, pid, w, 03);
			ptrace(WUREGS, pid, 2*(512+ps), uregs[ps]&amp;~020);
			lastbp = 0;
		}
		ptrace(CONTIN, pid, 0, 0);
		bpwait(1);
		w = uregs[pc]-2;
		for (i=0; i&lt;NBKP; i++)
			if (bkptl[i].loc == w)
				break;
		if (i &gt;= NBKP) {
			printf(&quot;%s\n&quot;, signals[signo]);
			return;
		}
		lastbp = w;
		ptrace(WUSER, pid, w, bkptl[i].ins);
		ptrace(WUREGS, pid, 2*(512+pc), w);
		ptrace(WUREGS, pid, 2*(512+ps), uregs[ps]|020);
		printf(&quot;Breakpoint: &quot;);
		psymoff(w, 0777);
		printf(&quot;\n&quot;);
		return;
	}
	error();
}

doexec()
{
	extern _exectrap;
	char *argl[32];
	register char *p, **ap;
	register c;

	_exectrap++;
	ap = argl;
	*ap++ = symfil;
	p = lp;
	do {
		while (*p==&#39; &#39;)
			p++;
		if (*p==&#39;\n&#39; || *p==&#39;\0&#39;)
			break;
		*ap++ = p;
		while (*p!=&#39; &#39; &amp;&amp; *p!=&#39;\n&#39;)
			p++;
		c = *p;
		*p++ = &#39;\0&#39;;
	} while (c != &#39;\n&#39;);
	*ap++ = 0;
	execv(symfil, argl);
}

setbp(runflag)
{
	register w, i1, l;
	int i2;

	for (w=0; w&lt;NBKP; w++) {
		l = bkptl[w].loc;
		if (l &amp;&amp; (runflag||bkptl[w].ins==0)) {
			i1 = ptrace(RUSER, pid, l, 0);
			if (i1==04567) {	/* jsr r5,... */
				i2 = ptrace(RUSER, pid, l+2, 0);
				if (loccsv == i2+l+4) { /* jsr r5,csv */
					l =+ 4;
					bkptl[w].loc = l;
					bkptl[w].flag =| BADJST;
					i1 = ptrace(RUSER, pid, l, 0);
				}
			}
			bkptl[w].ins = i1;
			ptrace(WUSER, pid, l, 03);
			if (errno) {
				printf(&quot;Can&#39;t set breakpoint &quot;);
				psymoff(bkptl[w].loc);
				printf(&quot;\n&quot;);
			}
		}
	}
}

bpwait(f)
{
	extern int onintr();
	register w;
	int stat;

    loop:
	signal(SIGINT, 1);
	while ((w = wait(&amp;stat))!=pid &amp;&amp; w != -1);
	signal(SIGINT, onintr);
	if (w == -1) {
		ptrace(EXIT, pid, 0, 0);
		pid = 0;
		printf(&quot;Wait error\n&quot;);
		reset();
	}
	if ((stat &amp; 0377) != 0177) {
		if (signo = stat&amp;0177)
			printf(&quot;%s\n&quot;, signals[signo]);
		printf(&quot;Process terminated.\n&quot;);
		if (pid == w) {
			pid = 0;
			reset();
		}
		goto loop;
	}
	signo = stat&gt;&gt;8;
	collinfo();
	if (signo!=SIGTRC) {
		printf(&quot;%s\n&quot;, signals[signo]);
		reset();
	}
}

collinfo()
{
	register i;

	for (i=0; i&lt;9; i++)
		uregs[reglist[i].roffs] =
		    ptrace(RUREGS, pid, 2*(512+reglist[i].roffs), 0);
	setstack();
}

symlook(symstr)
char *symstr;
{
	register i;
	register symv;

	symset();
	if (fsymbol[0]==0) {
		while(symget()) {
			if (eqstr(symbol, symstr)) {
				savsym();
				return(1);
			}
		}
		return(0);
	}
	while (symget()) {
		/* wait for function symbol */
		if (symbol[0]!=&#39;~&#39; || !eqstr(symbol, fsymbol))
			continue;
		symv = symval;
		while (symget()&amp;&amp; symbol[0]!=&#39;~&#39; &amp;&amp;symflg!=037)
			if (eqstr(symbol, symstr))
				return(localsym(symv));
		return(0);
	}
}

localsym(s)
{
	register i, xr5;

	/* label, static */
	if (symflg&gt;=2 &amp;&amp; symflg&lt;=4) {
		ssymval = symval;
		return(1);
	}
	/* auto, arg */
	if (symflg==1) {
		for (i=0; i&lt;callev; i++)
			if (entpt[i]==s) {
				ssymval = symval+callist[i];
				return(1);
			}
		return(0);
	}
	/* register */
	if (symflg==20) {
		for (i=0; i&lt;callev; i++)
			if (entpt[i]==s) {
				if (i==0) {
					return(0); /* temp, no reg lvalue */
				}
				ssymval = callist[i-1] - 10 + 2*symval;
				return(1);
			}
		return(0);
	}
	return(0);
}

eqstr(as1, as2)
int *as1, *as2;
{
	register char *s1, *s2, *es1;

	s1 = as1;
	s2 = as2;
	for (es1 = s1+8; s1 &lt; es1; )
		if (*s1++ != *s2++)
			return(0);
	return(1);
}

vallook(value)
char *value;
{
	register char *diff;

	diff = 0177777;
	symset();
	while (symget())
		if (symflg&amp;040 &amp;&amp; value-symval&lt;=diff) {
			if (symflg==1 &amp;&amp; value!=symval)
				continue;
			savsym(&#39;_&#39;);
			diff = value-symval;
		}
	return(diff);
}

get(aaddr, space)
char *aaddr;
{
	int w;
	register int w1;
	register char *addr;

	addr = aaddr;
	if (pid) {		/* tracing on? */
		w = ptrace(space==DSP?RUSER:RIUSER, pid, addr, 0);
		if (addr&amp;01) {
			w1 = ptrace(space==DSP?RUSER:RIUSER, pid, addr+1, 0);
			w = (w&gt;&gt;8)&amp;0377 | (w1&lt;&lt;8);
		}
		errflg = errno;
		return(w);
	}
	w = 0;
	if (idsep==0&amp;&amp;addr&lt;regbuf-&gt;u_tsize || idsep&amp;&amp;space==ISP) {
		seek(fsym, addr+020, 0);
		if (read(fsym, &amp;w, 2) != 2)
			errflg++;
		return(w);
	}
	if (addr &lt; rtsize+regbuf-&gt;u_dsize) {
		if (idsep==0)
			addr =- rtsize;
	} else if (-addr &lt; regbuf-&gt;u_ssize)
		addr =+ regbuf-&gt;u_dsize + regbuf-&gt;u_ssize;
	else
		errflg++;
	seek(fcore, addr+1024, 0);
	if (read(fcore, &amp;w, 2) &lt; 2)
		errflg++;
	return(w);
}

symset()
{
	symct = symlen;
	symptr = symbuf;
	seek(fsym, symoff, 0);
}

symget()
{
	register int *p, *q;
	if ((symct =- 12) &lt; 0)
		return(0);
	if (symptr &lt; &amp;symbuf[symcor]) {
		p = symptr;
		for (q=symbol; q &lt;= &amp;symval;)
			*q++ = *p++;
		symptr = p;
		return(1);
	}
	return(read(fsym, symbol, 12) == 12);
}

savsym(skip)
{
	register int ch;
	register char *p, *q;

	p = symbol;
	q = ssymbol;
	while (p&lt;symbol+8 &amp;&amp; (ch = *p++)) {
		if (ch == skip)
			continue;
		*q++ = ch;
	}
	while (q &lt; ssymbol+8)
		*q++ = &#39;\0&#39;;
	ssymflg = symflg;
	ssymval = symval;
}

onintr()
{
	putchar(&#39;\n&#39;);
	errflg++;
	reset();
}
