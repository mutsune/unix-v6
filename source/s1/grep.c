#
/*
 * grep -- print lines matching (or not matching) a pattern
 *
 */

#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11

#define	STAR	01

#define	LBSIZE	256
#define	ESIZE	256

char	ibuf[512];
char	expbuf[ESIZE];
int	lnum[2];
char	linebuf[LBSIZE+1];
int	bflag;
int	nflag;
int	cflag;
int	vflag;
int	nfile;
int	circf;
int	blkno;
int	tln[2];

main(argc, argv)
char **argv;
{
	extern fout;

	fout = dup(1);
	flush();
	while (--argc &gt; 0 &amp;&amp; (++argv)[0][0]==&#39;-&#39;)
		switch (argv[0][1]) {

		case &#39;v&#39;:
			vflag++;
			continue;

		case &#39;b&#39;:
			bflag++;
			continue;

		case &#39;c&#39;:
			cflag++;
			continue;

		case &#39;n&#39;:
			nflag++;
			continue;

		default:
			printf2(&quot;Unknown flag\n&quot;);
			continue;
		}
	if (argc&lt;=0)
		exit(2);
	compile(*argv);
	nfile = --argc;
	if (argc&lt;=0)
		execute(0);
	else while (--argc &gt;= 0) {
		argv++;
		execute(*argv);
	}
	flush();
	exit(0);
}

compile(astr)
char *astr;
{
	register c;
	register char *ep, *sp;
	char *lastep;
	int cclcnt;

	ep = expbuf;
	sp = astr;
	if (*sp == &#39;^&#39;) {
		circf++;
		sp++;
	}
	for (;;) {
		if (ep &gt;= &amp;expbuf[ESIZE])
			goto cerror;
		if ((c = *sp++) != &#39;*&#39;)
			lastep = ep;
		switch (c) {

		case &#39;\0&#39;:
			*ep++ = CEOF;
			return;

		case &#39;.&#39;:
			*ep++ = CDOT;
			continue;

		case &#39;*&#39;:
			if (lastep==0)
				goto defchar;
			*lastep =| STAR;
			continue;

		case &#39;$&#39;:
			if (*sp != &#39;\0&#39;)
				goto defchar;
			*ep++ = CDOL;
			continue;

		case &#39;[&#39;:
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c = *sp++) == &#39;^&#39;) {
				c = *sp++;
				ep[-2] = NCCL;
			}
			do {
				*ep++ = c;
				cclcnt++;
				if (c==&#39;\0&#39; || ep &gt;= &amp;expbuf[ESIZE])
					goto cerror;
			} while ((c = *sp++) != &#39;]&#39;);
			lastep[1] = cclcnt;
			continue;

		case &#39;\\&#39;:
			if ((c = *sp++) == &#39;\0&#39;)
				goto cerror;
		defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
    cerror:
	printf2(&quot;RE error\n&quot;);
}

execute(file)
{
	register char *p1, *p2;
	register c;
	int f;
	char *ebp, *cbp;

	if (file) {
		if ((f = open(file, 0)) &lt; 0) {
			printf2(&quot;Can&#39;t open %s\n&quot;, file);
		}
	} else
		f = 0;
	ebp = ibuf;
	cbp = ibuf;
	lnum[0] = 0;
	lnum[1] = 0;
	tln[0] = 0;
	tln[1] = 0;
	blkno = -1;
	for (;;) {
		if ((++lnum[1])==0)
			lnum[1]++;
		if((lnum[1]&amp;0377) == 0)
			flush();
		p1 = linebuf;
		p2 = cbp;
		for (;;) {
			if (p2 &gt;= ebp) {
				if ((c = read(f, ibuf, 512)) &lt;= 0) {
					close(f);
					if (cflag) {
						if (nfile &gt; 1)
							printf(&quot;%s:&quot;, file);
						p1 = locv(tln[0],tln[1]);
						printf(&quot;%s\n&quot;, p1);
					}
					return;
				}
				blkno++;
				p2 = ibuf;
				ebp = ibuf+c;
			}
			if ((c = *p2++) == &#39;\n&#39;)
				break;
			if(c)
			if (p1 &lt; &amp;linebuf[LBSIZE-1])
				*p1++ = c;
		}
		*p1++ = 0;
		cbp = p2;
		p1 = linebuf;
		p2 = expbuf;
		if (circf) {
			if (advance(p1, p2))
				goto found;
			goto nfound;
		}
		/* fast check for first character */
		if (*p2==CCHR) {
			c = p2[1];
			do {
				if (*p1!=c)
					continue;
				if (advance(p1, p2))
					goto found;
			} while (*p1++);
			goto nfound;
		}
		/* regular algorithm */
		do {
			if (advance(p1, p2))
				goto found;
		} while (*p1++);
	nfound:
		if (vflag)
			succeed(file);
		continue;
	found:
		if (vflag==0)
			succeed(file);
	}
}

advance(alp, aep)
{
	register char *lp, *ep, *curlp;
	char *nextep;

	lp = alp;
	ep = aep;
	for (;;) switch (*ep++) {

	case CCHR:
		if (*ep++ == *lp++)
			continue;
		return(0);

	case CDOT:
		if (*lp++)
			continue;
		return(0);

	case CDOL:
		if (*lp==0)
			continue;
		return(0);

	case CEOF:
		return(1);

	case CCL:
		if (cclass(ep, *lp++, 1)) {
			ep =+ *ep;
			continue;
		}
		return(0);

	case NCCL:
		if (cclass(ep, *lp++, 0)) {
			ep =+ *ep;
			continue;
		}
		return(0);

	case CDOT|STAR:
		curlp = lp;
		while (*lp++);
		goto star;

	case CCHR|STAR:
		curlp = lp;
		while (*lp++ == *ep);
		ep++;
		goto star;

	case CCL|STAR:
	case NCCL|STAR:
		curlp = lp;
		while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)));
		ep =+ *ep;
		goto star;

	star:
		do {
			lp--;
			if (advance(lp, ep))
				return(1);
		} while (lp &gt; curlp);
		return(0);

	default:
		printf2(&quot;RE botch\n&quot;);
	}
}

cclass(aset, ac, af)
{
	register char *set, c;
	register n;

	set = aset;
	if ((c = ac) == 0)
		return(0);
	n = *set++;
	while (--n)
		if (*set++ == c)
			return(af);
	return(!af);
}

printf2(s, a)
{
	extern fout;
	flush();
	fout = 2;
	printf(s, a);
	flush();
	exit(2);
}

succeed(f)
{
	if (cflag) {
		if (++tln[1]==0)
			tln[0]++;
		return;
	}
	if (nfile &gt; 1)
		printf(&quot;%s:&quot;, f);
	if (bflag)
		printf(&quot;%l:&quot;, blkno);
	if (nflag)
		printf(&quot;%s:&quot;, locv(lnum[0], lnum[1]));
	printf(&quot;%s\n&quot;, linebuf);
}
