#define	L	512
#define	N	7
#define	C	20
#define	MEM	(16*2048)
#define NF	10

int	ibuf[259];
int	obuf[259];
char	*file;
char	*filep;
int	nfiles;
int	nlines;
int	ntext;
int	*lspace;
char	*tspace;
int	cmp();
int	term();
int 	mflg;
char	*outfil;
char	tabchar;
int 	eargc;
char	**eargv;

char	fold[128] {
	0000,0001,0002,0003,0004,0005,0006,0007,
	0010,0011,0012,0013,0014,0015,0016,0017,
	0020,0021,0022,0023,0024,0025,0026,0027,
	0030,0031,0032,0033,0034,0035,0036,0037,
	0040,0041,0042,0043,0044,0045,0046,0047,
	0050,0051,0052,0053,0054,0055,0056,0057,
	0060,0061,0062,0063,0064,0065,0066,0067,
	0070,0071,0072,0073,0074,0075,0076,0077,
	0100,0101,0102,0103,0104,0105,0106,0107,
	0110,0111,0112,0113,0114,0115,0116,0117,
	0120,0121,0122,0123,0124,0125,0126,0127,
	0130,0131,0132,0133,0134,0134,0136,0137,
	0140,0101,0102,0103,0104,0105,0106,0107,
	0110,0111,0112,0113,0114,0115,0116,0117,
	0120,0121,0122,0123,0124,0125,0126,0127,
	0130,0131,0132,0173,0174,0175,0176,0177
};
char	nofold[128];
char	dict[128] {
	1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1
};
char	nodict[128] { 1 };

struct	field {
	char *code;
	char *ignore;
	int nflg;
	int rflg;
	int bflg;
	char *m[2];
	char *n[2];
}	fields[NF];
int proto[9] {
	&amp;fold,
	&amp;nodict,
	0,
	1,
	0,
	0,-1,
	0,0
};
int	nfields;
int 	error 1;

main(argc, argv)
char **argv;
{
	register a, i;
	char *arg;
	register int *p;
	int *q;

	for(a=0; a&lt;128; a++)
		nofold[a] = a;
	nodict[127] = 1;
	copyproto();
	eargv = argv;
	while (--argc &gt; 0) {
		if(**++argv == &#39;-&#39;) for(arg = *argv;;) {
			switch(*++arg) {
			case &#39;\0&#39;:
				if(arg[-1] == &#39;-&#39;)
					eargv[eargc++] = &quot;-&quot;;
				break;

			case &#39;m&#39;:
				mflg++;
				continue;

			case &#39;o&#39;:
				if(--argc &gt; 0)
					outfil = *++argv;
				continue;

			default:
				field(++*argv,1);
				break;
			}
			break;
		} else if (**argv == &#39;+&#39;) {
			if(++nfields&gt;=NF) {
				mess(&quot;Too many keys\n&quot;);
				exit(1);
			}
			copyproto();
			field(++*argv,0);
		} else
			eargv[eargc++] = *argv;
	}
	q = &amp;fields[0];
	for(a=1; a&lt;=nfields; a++) {
		p = &amp;fields[a];
		for(i=0; i&lt;5; i++)	/*sensitive to sizeof(proto)*/
			if(p[i] != proto[i])
				goto next;
		for(i=0; i&lt;5; i++)
			p[i] = q[i];
next:	;
	}
	if(eargc == 0)
		eargv[eargc++] = &quot;-&quot;;

	a = MEM;
	i = lspace = sbrk(0);
	while(brk(a) == -1)
		a =- 512;
	brk(a =- 512);	/* for recursion */
	a =- i;
	nlines = ((a-L)&gt;&gt;1) &amp; 077777;
	nlines =/ 5;
	ntext = nlines*8;
	tspace = lspace+nlines;
	file = &quot;/usr/tmp/stmXaa&quot;;
loop:
	filep = file;
	while(*filep != &#39;X&#39;)
		filep++;
	for(*filep = &#39;a&#39;;;(*filep)++) {
		if(stat(file, lspace) &lt; 0) {
			a = creat(file, 0600);
			if(a &gt;= 0)
				break;
		}
		if(*filep == &#39;z&#39;) {
			if(file[1] != &#39;t&#39;) {
				file = &quot;/tmp/stmXaa&quot;;
				goto loop;
			}
			mess(&quot;Cannot locate temp\n&quot;);
			exit(1);
		}
	}
	close(a);
	filep++;
	if ((signal(2, 1) &amp; 01) == 0)
		signal(2, term);
	nfiles = eargc;
	if(!mflg) {
		ibuf[0] = -1;
		sort();
		close(0);
	}
	for(a = mflg?0:eargc; a+N &lt; nfiles; a=+N) {
		newfile();
		merge(a, a+N);
	}
	if(a != nfiles) {
		oldfile();
		merge(a, nfiles);
	}
	error = 0;
	term();
}

sort()
{
	register char *cp;
	register *lp, c;
	int done;
	int i;
	int f;

	done = 0;
	i = 0;
	do {
		cp = tspace;
		lp = lspace;
		while(lp &lt; lspace+nlines &amp;&amp; cp &lt; tspace+ntext) {
			*lp++ = cp;
			while((*cp++ = c = getc(ibuf)) != &#39;\n&#39;) {
				if(c &gt;= 0) continue;
				cp--;
				close(ibuf[0]);
				if(i &lt; eargc) {
					if((f = setfil(i++)) == 0)
						ibuf[0] = 0;
					else if(fopen(f, ibuf) &lt; 0)
						cant(f);
				} else
					break;
			}
			if(c &lt; 0) {
				done++;
				lp--;
				break;
			}
		}
		qsort(lspace, lp-lspace, 2, cmp);
		if(done == 0 || nfiles != eargc)
			newfile(); else
			oldfile();
		while(lp &gt; lspace) {
			cp = *--lp;
			if(*cp)
				do
				putc(*cp, obuf);
				while(*cp++ != &#39;\n&#39;);
		}
		fflush(obuf);
		close(obuf[0]);
	} while(done == 0);
}

struct merg
{
	char	l[L];
	int	b[259];
};

merge(a, b)
{
	register struct merg *p;
	register char *cp;
	register i;
	struct { int *ip;};
	int f;
	int j;
	int	k,l,c;

	p = lspace;
	j = 0;
	for(i=a; i&lt;b; i++) {
		f = setfil(i);
		if(f == 0)
			p-&gt;b[0] = dup(0);
		else if(fopen(f, p-&gt;b) &lt; 0)
			cant(f);
		ibuf[j] = p;
		if(!rline(p)) j++;
		p++;
	}

	do {
		i = j;
		qsort(ibuf, i, 2, cmp);
		l = 0;
		while(i--) {
			cp = ibuf[i];
			if(*cp == &#39;\0&#39;) {
				l = 1;
				if(rline(ibuf[i])) {
					k = i;
					while(++k &lt; j)
						ibuf[k-1] = ibuf[k];
					j--;
				}
			}
		}
	} while(l);

	i = j;
	if(i &gt; 0) for(;;) {
		cp = ibuf[i-1];
		if(i == 1 || cmp(&amp;ibuf[i-1], &amp;ibuf[i-2]))
			do
				putc(*cp, obuf);
			while(*cp++ != &#39;\n&#39;);
		if(rline(ibuf[i-1])) {
			i--;
			if(i == 0)
				break;
		}
		cp = &amp;ibuf[i];
		while (--cp.ip &gt; ibuf &amp;&amp; cmp(cp.ip, cp.ip-1) &lt; 0) {
			p = *cp.ip;
			*cp.ip = *(cp.ip-1);
			*(cp.ip-1) = p;
		}
	}
	p = lspace;
	for(i=a; i&lt;b; i++) {
		close(p-&gt;b[0]);
		p++;
		if(i &gt;= eargc)
			close(creat(setfil(i)));
	}
	fflush(obuf);
	close(obuf[0]);
}

rline(mp)
struct merg *mp;
{
	register char *cp;
	register *bp, c;

	bp = mp-&gt;b;
	cp = mp-&gt;l;
	do {
		c = getc(bp);
		if(c &lt; 0)
			return(1);
		*cp++ = c;
	} while(c != &#39;\n&#39;);
	*cp = &#39;\0&#39;;
	return(0);
}

newfile()
{

	if(fcreat(setfil(nfiles), obuf) &lt; 0) {
		mess(&quot;Can&#39;t create temp\n&quot;);
		term();
	}
	nfiles++;
}

char *
setfil(i)
{

	if(i &lt; eargc)
		if(eargv[i][0] == &#39;-&#39; &amp;&amp; eargv[i][1] == &#39;\0&#39;)
			return(0);
		else
			return(eargv[i]);
	i =- eargc;
	filep[0] = i/26 + &#39;a&#39;;
	filep[1] = i%26 + &#39;a&#39;;
	return(file);
}

oldfile()
{

	if(outfil) {
		if(fcreat(outfil, obuf) &lt; 0) {
			mess(&quot;Can&#39;t create output\n&quot;);
			term();
		}
	} else
		obuf[0] = 1;
}

cant(f)
{
	mess(&quot;Can&#39;t open &quot;);
	mess(f);
	mess(&quot;\n&quot;);
	term();
}

term()
{
	register i;

	if(nfiles == eargc)
		nfiles++;
	for(i=eargc; i&lt;nfiles; i++) {
		unlink(setfil(i));
	}
	exit(error);
}

cmp(a, b)
int	*a,*b;
{
	register char	*ra, *rb;

	ra = *a - 1;
	rb = *b - 1;

	while(*++ra == *++rb)
		if(*ra == &#39;\n&#39;)
			return(0);

	return(*rb - *ra);
}


skip(pp, fp, j)
struct field *fp;
char *pp;
{
	register i;
	register char *p;

	p = pp;
	if( (i=fp-&gt;m[j]) &lt; 0)
		return(-1);
	while(i-- &gt; 0) {
		if(tabchar != 0) {
			while(*p != tabchar)
				if(*p != &#39;\n&#39;)
					p++;
				else goto ret;
			p++;
		} else {
			while(blank(*p))
				p++;
			while(!blank(*p))
				if(*p != &#39;\n&#39;)
					p++;
				else goto ret;
		}
	}
	if(fp-&gt;bflg)
		while(blank(*p))
			p++;
	i = fp-&gt;n[j];
	while(i-- &gt; 0) {
		if(*p != &#39;\n&#39;)
			p++;
		else goto ret;
	} 
ret:
	return(p);
}

digit(c)
{

	return(c &lt;= &#39;9&#39; &amp;&amp; c &gt;= &#39;0&#39;);
}

mess(s)
char *s;
{
	while(*s)
		write(2, s++, 1);
}

copyproto()
{
	register int i, *p, *q;

	p = proto;
	q = &amp;fields[nfields];
	for(i=0; i&lt;sizeof(proto)/2; i++)
		*q++ = *p++;
}

field(s,k)
char *s;
{
	register struct field *p;
	p = &amp;fields[nfields];
	for(; *s!=0; s++) {
		switch(*s) {
		case &#39;\0&#39;:
			return;

		case &#39;a&#39;:
			p-&gt;code = nofold;
			break;

		case &#39;b&#39;:
			p-&gt;bflg++;
			break;

		case &#39;d&#39;:
			p-&gt;ignore = dict;
			break;

		case &#39;n&#39;:
			p-&gt;nflg++;
			break;
		case &#39;t&#39;:
			tabchar = *++s;
			if(tabchar == 0) s--;
			break;

		case &#39;r&#39;:
			p-&gt;rflg = -1;
			break;

		default:
			p-&gt;m[k] = number(&amp;s);
			if(*s == &#39;.&#39;)
				s++;
			p-&gt;n[k] = number(&amp;s);
			s--;
		}
	}
}

number(ppa)
char **ppa;
{
	int n;
	register char *pa;
	pa = *ppa;
	n = 0;
	while(digit(*pa))
		n = n*10 + *pa++ - &#39;0&#39;;
	*ppa = pa;
	return(n);
}

blank(c)
{
	if(c==&#39; &#39; || c==&#39;\t&#39;)
		return(1);
	return(0);
}

int	(*qscmp)();
int	qses;

qsort(a, n, es, fc)
char *a;
int n, es;
int (*fc)();
{
	qscmp = fc;
	qses = es;
	qs1(a, a+n*es);
}

qs1(a, l)
char *a, *l;
{
	register char *i, *j, *es;
	char **k;
	char *lp, *hp;
	int n, c;


	es = qses;

start:
	if((n=l-a) &lt;= es)
		return;


	n = ((n/(2*es))*es) &amp; 077777;
	hp = lp = a+n;
	i = a;
	j = l-es;


	for(;;) {
		if(i &lt; lp) {
			if((c = (*qscmp)(i, lp)) == 0) {
				qsexc(i, lp =- es);
				continue;
			}
			if(c &lt; 0) {
				i =+ es;
				continue;
			}
		}

loop:
		if(j &gt; hp) {
			if((c = (*qscmp)(hp, j)) == 0) {
				qsexc(hp =+ es, j);
				goto loop;
			}
			if(c &gt; 0) {
				if(i == lp) {
					qstexc(i, hp =+ es, j);
					i = lp =+ es;
					goto loop;
				}
				qsexc(i, j);
				j =- es;
				i =+ es;
				continue;
			}
			j =- es;
			goto loop;
		}


		if(i == lp) {
			for(k=lp+2; k&lt;=hp;) *(*k++)=&#39;\0&#39;;
			if(lp-a &gt;= l-hp) {
				qs1(hp+es, l);
				l = lp;
			} else {
				qs1(a, lp);
				a = hp+es;
			}
			goto start;
		}


		qstexc(j, lp =- es, i);
		j = hp =- es;
	}
}

qsexc(i, j)
char *i, *j;
{
	register char *ri, *rj, c;
	int n;

	n = qses;
	ri = i;
	rj = j;
	do {
		c = *ri;
		*ri++ = *rj;
		*rj++ = c;
	} while(--n);
}

qstexc(i, j, k)
char *i, *j, *k;
{
	register char *ri, *rj, *rk;
	char	c;
	int	n;

	n = qses;
	ri = i;
	rj = j;
	rk = k;
	do {
		c = *ri;
		*ri++ = *rk;
		*rk++ = *rj;
		*rj++ = c;
	} while(--n);
}
