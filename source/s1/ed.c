#

/*
 * Editor
 */

#define	SIGHUP	1
#define	SIGINTR	2
#define	SIGQUIT	3
#define	FNSIZE	64
#define	LBSIZE	512
#define	ESIZE	128
#define	GBSIZE	256
#define	NBRA	5
#define	EOF	-1

#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12

#define	STAR	01

#define	error	goto errlab
#define	READ	0
#define	WRITE	1

int	peekc;
int	lastc;
char	savedfile[FNSIZE];
char	file[FNSIZE];
char	linebuf[LBSIZE];
char	rhsbuf[LBSIZE/2];
char	expbuf[ESIZE+4];
int	circfl;
int	*zero;
int	*dot;
int	*dol;
int	*endcore;
int	*fendcore;
int	*addr1;
int	*addr2;
char	genbuf[LBSIZE];
int	count[2];
char	*nextip;
char	*linebp;
int	ninbuf;
int	io;
int	pflag;
int	onhup;
int	onquit;
int	vflag	1;
int	listf;
int	col;
char	*globp;
int	tfile	-1;
int	tline;
char	*tfname;
char	*loc1;
char	*loc2;
char	*locs;
char	ibuff[512];
int	iblock	-1;
char	obuff[512];
int	oblock	-1;
int	ichanged;
int	nleft;
int	errfunc();
int	*errlab	errfunc;
char	TMPERR[] &quot;TMP&quot;;
int	names[26];
char	*braslist[NBRA];
char	*braelist[NBRA];

main(argc, argv)
char **argv;
{
	register char *p1, *p2;
	extern int onintr();

	onquit = signal(SIGQUIT, 1);
	onhup = signal(SIGHUP, 1);
	argv++;
	if (argc &gt; 1 &amp;&amp; **argv==&#39;-&#39;) {
		vflag = 0;
		/* allow debugging quits? */
		if ((*argv)[1]==&#39;q&#39;) {
			signal(SIGQUIT, 0);
			vflag++;
		}
		argv++;
		argc--;
	}
	if (argc&gt;1) {
		p1 = *argv;
		p2 = savedfile;
		while (*p2++ = *p1++);
		globp = &quot;r&quot;;
	}
	fendcore = sbrk(0);
	init();
	if ((signal(SIGINTR, 1) &amp; 01) == 0)
		signal(SIGINTR, onintr);
	setexit();
	commands();
	unlink(tfname);
}

commands()
{
	int getfile(), gettty();
	register *a1, c;
	register char *p;
	int r;

	for (;;) {
	if (pflag) {
		pflag = 0;
		addr1 = addr2 = dot;
		goto print;
	}
	addr1 = 0;
	addr2 = 0;
	do {
		addr1 = addr2;
		if ((a1 = address())==0) {
			c = getchar();
			break;
		}
		addr2 = a1;
		if ((c=getchar()) == &#39;;&#39;) {
			c = &#39;,&#39;;
			dot = a1;
		}
	} while (c==&#39;,&#39;);
	if (addr1==0)
		addr1 = addr2;
	switch(c) {

	case &#39;a&#39;:
		setdot();
		newline();
		append(gettty, addr2);
		continue;

	case &#39;c&#39;:
		delete();
		append(gettty, addr1-1);
		continue;

	case &#39;d&#39;:
		delete();
		continue;

	case &#39;e&#39;:
		setnoaddr();
		if ((peekc = getchar()) != &#39; &#39;)
			error;
		savedfile[0] = 0;
		init();
		addr2 = zero;
		goto caseread;

	case &#39;f&#39;:
		setnoaddr();
		if ((c = getchar()) != &#39;\n&#39;) {
			peekc = c;
			savedfile[0] = 0;
			filename();
		}
		puts(savedfile);
		continue;

	case &#39;g&#39;:
		global(1);
		continue;

	case &#39;i&#39;:
		setdot();
		nonzero();
		newline();
		append(gettty, addr2-1);
		continue;

	case &#39;k&#39;:
		if ((c = getchar()) &lt; &#39;a&#39; || c &gt; &#39;z&#39;)
			error;
		newline();
		setdot();
		nonzero();
		names[c-&#39;a&#39;] = *addr2 | 01;
		continue;

	case &#39;m&#39;:
		move(0);
		continue;

	case &#39;\n&#39;:
		if (addr2==0)
			addr2 = dot+1;
		addr1 = addr2;
		goto print;

	case &#39;l&#39;:
		listf++;
	case &#39;p&#39;:
		newline();
	print:
		setdot();
		nonzero();
		a1 = addr1;
		do
			puts(getline(*a1++));
		while (a1 &lt;= addr2);
		dot = addr2;
		listf = 0;
		continue;

	case &#39;q&#39;:
		setnoaddr();
		newline();
		unlink(tfname);
		exit();

	case &#39;r&#39;:
	caseread:
		filename();
		if ((io = open(file, 0)) &lt; 0) {
			lastc = &#39;\n&#39;;
			error;
		}
		setall();
		ninbuf = 0;
		append(getfile, addr2);
		exfile();
		continue;

	case &#39;s&#39;:
		setdot();
		nonzero();
		substitute(globp);
		continue;

	case &#39;t&#39;:
		move(1);
		continue;

	case &#39;v&#39;:
		global(0);
		continue;

	case &#39;w&#39;:
		setall();
		nonzero();
		filename();
		if ((io = creat(file, 0666)) &lt; 0)
			error;
		putfile();
		exfile();
		continue;

	case &#39;=&#39;:
		setall();
		newline();
		count[1] = (addr2-zero)&amp;077777;
		putd();
		putchar(&#39;\n&#39;);
		continue;

	case &#39;!&#39;:
		unix();
		continue;

	case EOF:
		return;

	}
	error;
	}
}

address()
{
	register *a1, minus, c;
	int n, relerr;

	minus = 0;
	a1 = 0;
	for (;;) {
		c = getchar();
		if (&#39;0&#39;&lt;=c &amp;&amp; c&lt;=&#39;9&#39;) {
			n = 0;
			do {
				n =* 10;
				n =+ c - &#39;0&#39;;
			} while ((c = getchar())&gt;=&#39;0&#39; &amp;&amp; c&lt;=&#39;9&#39;);
			peekc = c;
			if (a1==0)
				a1 = zero;
			if (minus&lt;0)
				n = -n;
			a1 =+ n;
			minus = 0;
			continue;
		}
		relerr = 0;
		if (a1 || minus)
			relerr++;
		switch(c) {
		case &#39; &#39;:
		case &#39;\t&#39;:
			continue;
	
		case &#39;+&#39;:
			minus++;
			if (a1==0)
				a1 = dot;
			continue;

		case &#39;-&#39;:
		case &#39;^&#39;:
			minus--;
			if (a1==0)
				a1 = dot;
			continue;
	
		case &#39;?&#39;:
		case &#39;/&#39;:
			compile(c);
			a1 = dot;
			for (;;) {
				if (c==&#39;/&#39;) {
					a1++;
					if (a1 &gt; dol)
						a1 = zero;
				} else {
					a1--;
					if (a1 &lt; zero)
						a1 = dol;
				}
				if (execute(0, a1))
					break;
				if (a1==dot)
					error;
			}
			break;
	
		case &#39;$&#39;:
			a1 = dol;
			break;
	
		case &#39;.&#39;:
			a1 = dot;
			break;

		case &#39;\&#39;&#39;:
			if ((c = getchar()) &lt; &#39;a&#39; || c &gt; &#39;z&#39;)
				error;
			for (a1=zero; a1&lt;=dol; a1++)
				if (names[c-&#39;a&#39;] == (*a1|01))
					break;
			break;
	
		default:
			peekc = c;
			if (a1==0)
				return(0);
			a1 =+ minus;
			if (a1&lt;zero || a1&gt;dol)
				error;
			return(a1);
		}
		if (relerr)
			error;
	}
}

setdot()
{
	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 &gt; addr2)
		error;
}

setall()
{
	if (addr2==0) {
		addr1 = zero+1;
		addr2 = dol;
		if (dol==zero)
			addr1 = zero;
	}
	setdot();
}

setnoaddr()
{
	if (addr2)
		error;
}

nonzero()
{
	if (addr1&lt;=zero || addr2&gt;dol)
		error;
}

newline()
{
	register c;

	if ((c = getchar()) == &#39;\n&#39;)
		return;
	if (c==&#39;p&#39; || c==&#39;l&#39;) {
		pflag++;
		if (c==&#39;l&#39;)
			listf++;
		if (getchar() == &#39;\n&#39;)
			return;
	}
	error;
}

filename()
{
	register char *p1, *p2;
	register c;

	count[1] = 0;
	c = getchar();
	if (c==&#39;\n&#39; || c==EOF) {
		p1 = savedfile;
		if (*p1==0)
			error;
		p2 = file;
		while (*p2++ = *p1++);
		return;
	}
	if (c!=&#39; &#39;)
		error;
	while ((c = getchar()) == &#39; &#39;);
	if (c==&#39;\n&#39;)
		error;
	p1 = file;
	do {
		*p1++ = c;
	} while ((c = getchar()) != &#39;\n&#39;);
	*p1++ = 0;
	if (savedfile[0]==0) {
		p1 = savedfile;
		p2 = file;
		while (*p1++ = *p2++);
	}
}

exfile()
{
	close(io);
	io = -1;
	if (vflag) {
		putd();
		putchar(&#39;\n&#39;);
	}
}

onintr()
{
	signal(SIGINTR, onintr);
	putchar(&#39;\n&#39;);
	lastc = &#39;\n&#39;;
	error;
}

errfunc()
{
	register c;

	listf = 0;
	puts(&quot;?&quot;);
	count[0] = 0;
	seek(0, 0, 2);
	pflag = 0;
	if (globp)
		lastc = &#39;\n&#39;;
	globp = 0;
	peekc = lastc;
	while ((c = getchar()) != &#39;\n&#39; &amp;&amp; c != EOF);
	if (io &gt; 0) {
		close(io);
		io = -1;
	}
	reset();
}

getchar()
{
	if (lastc=peekc) {
		peekc = 0;
		return(lastc);
	}
	if (globp) {
		if ((lastc = *globp++) != 0)
			return(lastc);
		globp = 0;
		return(EOF);
	}
	if (read(0, &amp;lastc, 1) &lt;= 0)
		return(lastc = EOF);
	lastc =&amp; 0177;
	return(lastc);
}

gettty()
{
	register c, gf;
	register char *p;

	p = linebuf;
	gf = globp;
	while ((c = getchar()) != &#39;\n&#39;) {
		if (c==EOF) {
			if (gf)
				peekc = c;
			return(c);
		}
		if ((c =&amp; 0177) == 0)
			continue;
		*p++ = c;
		if (p &gt;= &amp;linebuf[LBSIZE-2])
			error;
	}
	*p++ = 0;
	if (linebuf[0]==&#39;.&#39; &amp;&amp; linebuf[1]==0)
		return(EOF);
	return(0);
}

getfile()
{
	register c;
	register char *lp, *fp;

	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf &lt; 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) &lt; 0)
				return(EOF);
			fp = genbuf;
		}
		if (lp &gt;= &amp;linebuf[LBSIZE])
			error;
		if ((*lp++ = c = *fp++ &amp; 0177) == 0) {
			lp--;
			continue;
		}
		if (++count[1] == 0)
			++count[0];
	} while (c != &#39;\n&#39;);
	*--lp = 0;
	nextip = fp;
	return(0);
}

putfile()
{
	int *a1;
	register char *fp, *lp;
	register nib;

	nib = 512;
	fp = genbuf;
	a1 = addr1;
	do {
		lp = getline(*a1++);
		for (;;) {
			if (--nib &lt; 0) {
				write(io, genbuf, fp-genbuf);
				nib = 511;
				fp = genbuf;
			}
			if (++count[1] == 0)
				++count[0];
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = &#39;\n&#39;;
				break;
			}
		}
	} while (a1 &lt;= addr2);
	write(io, genbuf, fp-genbuf);
}

append(f, a)
int (*f)();
{
	register *a1, *a2, *rdot;
	int nline, tl;
	struct { int integer; };

	nline = 0;
	dot = a;
	while ((*f)() == 0) {
		if (dol &gt;= endcore) {
			if (sbrk(1024) == -1)
				error;
			endcore.integer =+ 1024;
		}
		tl = putline();
		nline++;
		a1 = ++dol;
		a2 = a1+1;
		rdot = ++dot;
		while (a1 &gt; rdot)
			*--a2 = *--a1;
		*rdot = tl;
	}
	return(nline);
}

unix()
{
	register savint, pid, rpid;
	int retcode;

	setnoaddr();
	if ((pid = fork()) == 0) {
		signal(SIGHUP, onhup);
		signal(SIGQUIT, onquit);
		execl(&quot;/bin/sh&quot;, &quot;sh&quot;, &quot;-t&quot;, 0);
		exit();
	}
	savint = signal(SIGINTR, 1);
	while ((rpid = wait(&amp;retcode)) != pid &amp;&amp; rpid != -1);
	signal(SIGINTR, savint);
	puts(&quot;!&quot;);
}

delete()
{
	register *a1, *a2, *a3;

	setdot();
	newline();
	nonzero();
	a1 = addr1;
	a2 = addr2+1;
	a3 = dol;
	dol =- a2 - a1;
	do
		*a1++ = *a2++;
	while (a2 &lt;= a3);
	a1 = addr1;
	if (a1 &gt; dol)
		a1 = dol;
	dot = a1;
}

getline(tl)
{
	register char *bp, *lp;
	register nl;

	lp = linebuf;
	bp = getblock(tl, READ);
	nl = nleft;
	tl =&amp; ~0377;
	while (*lp++ = *bp++)
		if (--nl == 0) {
			bp = getblock(tl=+0400, READ);
			nl = nleft;
		}
	return(linebuf);
}

putline()
{
	register char *bp, *lp;
	register nl;
	int tl;

	lp = linebuf;
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl =&amp; ~0377;
	while (*bp = *lp++) {
		if (*bp++ == &#39;\n&#39;) {
			*--bp = 0;
			linebp = lp;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl=+0400, WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline =+ (((lp-linebuf)+03)&gt;&gt;1)&amp;077776;
	return(nl);
}

getblock(atl, iof)
{
	extern read(), write();
	register bno, off;
	
	bno = (atl&gt;&gt;8)&amp;0377;
	off = (atl&lt;&lt;1)&amp;0774;
	if (bno &gt;= 255) {
		puts(TMPERR);
		error;
	}
	nleft = 512 - off;
	if (bno==iblock) {
		ichanged =| iof;
		return(ibuff+off);
	}
	if (bno==oblock)
		return(obuff+off);
	if (iof==READ) {
		if (ichanged)
			blkio(iblock, ibuff, write);
		ichanged = 0;
		iblock = bno;
		blkio(bno, ibuff, read);
		return(ibuff+off);
	}
	if (oblock&gt;=0)
		blkio(oblock, obuff, write);
	oblock = bno;
	return(obuff+off);
}

blkio(b, buf, iofcn)
int (*iofcn)();
{
	seek(tfile, b, 3);
	if ((*iofcn)(tfile, buf, 512) != 512) {
		puts(TMPERR);
		error;
	}
}

init()
{
	register char *p;
	register pid;

	close(tfile);
	tline = 0;
	iblock = -1;
	oblock = -1;
	tfname = &quot;/tmp/exxxxx&quot;;
	ichanged = 0;
	pid = getpid();
	for (p = &amp;tfname[11]; p &gt; &amp;tfname[6];) {
		*--p = (pid&amp;07) + &#39;0&#39;;
		pid =&gt;&gt; 3;
	}
	close(creat(tfname, 0600));
	tfile = open(tfname, 2);
	brk(fendcore);
	dot = zero = dol = fendcore;
	endcore = fendcore - 2;
}

global(k)
{
	register char *gp;
	register c;
	register int *a1;
	char globuf[GBSIZE];

	if (globp)
		error;
	setall();
	nonzero();
	if ((c=getchar())==&#39;\n&#39;)
		error;
	compile(c);
	gp = globuf;
	while ((c = getchar()) != &#39;\n&#39;) {
		if (c==EOF)
			error;
		if (c==&#39;\\&#39;) {
			c = getchar();
			if (c!=&#39;\n&#39;)
				*gp++ = &#39;\\&#39;;
		}
		*gp++ = c;
		if (gp &gt;= &amp;globuf[GBSIZE-2])
			error;
	}
	*gp++ = &#39;\n&#39;;
	*gp++ = 0;
	for (a1=zero; a1&lt;=dol; a1++) {
		*a1 =&amp; ~01;
		if (a1&gt;=addr1 &amp;&amp; a1&lt;=addr2 &amp;&amp; execute(0, a1)==k)
			*a1 =| 01;
	}
	for (a1=zero; a1&lt;=dol; a1++) {
		if (*a1 &amp; 01) {
			*a1 =&amp; ~01;
			dot = a1;
			globp = globuf;
			commands();
			a1 = zero;
		}
	}
}

substitute(inglob)
{
	register gsubf, *a1, nl;
	int getsub();

	gsubf = compsub();
	for (a1 = addr1; a1 &lt;= addr2; a1++) {
		if (execute(0, a1)==0)
			continue;
		inglob =| 01;
		dosub();
		if (gsubf) {
			while (*loc2) {
				if (execute(1)==0)
					break;
				dosub();
			}
		}
		*a1 = putline();
		nl = append(getsub, a1);
		a1 =+ nl;
		addr2 =+ nl;
	}
	if (inglob==0)
		error;
}

compsub()
{
	register seof, c;
	register char *p;
	int gsubf;

	if ((seof = getchar()) == &#39;\n&#39;)
		error;
	compile(seof);
	p = rhsbuf;
	for (;;) {
		c = getchar();
		if (c==&#39;\\&#39;)
			c = getchar() | 0200;
		if (c==&#39;\n&#39;)
			error;
		if (c==seof)
			break;
		*p++ = c;
		if (p &gt;= &amp;rhsbuf[LBSIZE/2])
			error;
	}
	*p++ = 0;
	if ((peekc = getchar()) == &#39;g&#39;) {
		peekc = 0;
		newline();
		return(1);
	}
	newline();
	return(0);
}

getsub()
{
	register char *p1, *p2;

	p1 = linebuf;
	if ((p2 = linebp) == 0)
		return(EOF);
	while (*p1++ = *p2++);
	linebp = 0;
	return(0);
}

dosub()
{
	register char *lp, *sp, *rp;
	int c;

	lp = linebuf;
	sp = genbuf;
	rp = rhsbuf;
	while (lp &lt; loc1)
		*sp++ = *lp++;
	while (c = *rp++) {
		if (c==&#39;&amp;&#39;) {
			sp = place(sp, loc1, loc2);
			continue;
		} else if (c&lt;0 &amp;&amp; (c =&amp; 0177) &gt;=&#39;1&#39; &amp;&amp; c &lt; NBRA+&#39;1&#39;) {
			sp = place(sp, braslist[c-&#39;1&#39;], braelist[c-&#39;1&#39;]);
			continue;
		}
		*sp++ = c&amp;0177;
		if (sp &gt;= &amp;genbuf[LBSIZE])
			error;
	}
	lp = loc2;
	loc2 = sp + linebuf - genbuf;
	while (*sp++ = *lp++)
		if (sp &gt;= &amp;genbuf[LBSIZE])
			error;
	lp = linebuf;
	sp = genbuf;
	while (*lp++ = *sp++);
}

place(asp, al1, al2)
{
	register char *sp, *l1, *l2;

	sp = asp;
	l1 = al1;
	l2 = al2;
	while (l1 &lt; l2) {
		*sp++ = *l1++;
		if (sp &gt;= &amp;genbuf[LBSIZE])
			error;
	}
	return(sp);
}

move(cflag)
{
	register int *adt, *ad1, *ad2;
	int getcopy();

	setdot();
	nonzero();
	if ((adt = address())==0)
		error;
	newline();
	ad1 = addr1;
	ad2 = addr2;
	if (cflag) {
		ad1 = dol;
		append(getcopy, ad1++);
		ad2 = dol;
	}
	ad2++;
	if (adt&lt;ad1) {
		dot = adt + (ad2-ad1);
		if ((++adt)==ad1)
			return;
		reverse(adt, ad1);
		reverse(ad1, ad2);
		reverse(adt, ad2);
	} else if (adt &gt;= ad2) {
		dot = adt++;
		reverse(ad1, ad2);
		reverse(ad2, adt);
		reverse(ad1, adt);
	} else
		error;
}

reverse(aa1, aa2)
{
	register int *a1, *a2, t;

	a1 = aa1;
	a2 = aa2;
	for (;;) {
		t = *--a2;
		if (a2 &lt;= a1)
			return;
		*a2 = *a1;
		*a1++ = t;
	}
}

getcopy()
{
	if (addr1 &gt; addr2)
		return(EOF);
	getline(*addr1++);
	return(0);
}

compile(aeof)
{
	register eof, c;
	register char *ep;
	char *lastep;
	char bracket[NBRA], *bracketp;
	int nbra;
	int cclcnt;

	ep = expbuf;
	eof = aeof;
	bracketp = bracket;
	nbra = 0;
	if ((c = getchar()) == eof) {
		if (*ep==0)
			error;
		return;
	}
	circfl = 0;
	if (c==&#39;^&#39;) {
		c = getchar();
		circfl++;
	}
	if (c==&#39;*&#39;)
		goto cerror;
	peekc = c;
	for (;;) {
		if (ep &gt;= &amp;expbuf[ESIZE])
			goto cerror;
		c = getchar();
		if (c==eof) {
			*ep++ = CEOF;
			return;
		}
		if (c!=&#39;*&#39;)
			lastep = ep;
		switch (c) {

		case &#39;\\&#39;:
			if ((c = getchar())==&#39;(&#39;) {
				if (nbra &gt;= NBRA)
					goto cerror;
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;
			}
			if (c == &#39;)&#39;) {
				if (bracketp &lt;= bracket)
					goto cerror;
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;
			}
			*ep++ = CCHR;
			if (c==&#39;\n&#39;)
				goto cerror;
			*ep++ = c;
			continue;

		case &#39;.&#39;:
			*ep++ = CDOT;
			continue;

		case &#39;\n&#39;:
			goto cerror;

		case &#39;*&#39;:
			if (*lastep==CBRA || *lastep==CKET)
				error;
			*lastep =| STAR;
			continue;

		case &#39;$&#39;:
			if ((peekc=getchar()) != eof)
				goto defchar;
			*ep++ = CDOL;
			continue;

		case &#39;[&#39;:
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c=getchar()) == &#39;^&#39;) {
				c = getchar();
				ep[-2] = NCCL;
			}
			do {
				if (c==&#39;\n&#39;)
					goto cerror;
				*ep++ = c;
				cclcnt++;
				if (ep &gt;= &amp;expbuf[ESIZE])
					goto cerror;
			} while ((c = getchar()) != &#39;]&#39;);
			lastep[1] = cclcnt;
			continue;

		defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
   cerror:
	expbuf[0] = 0;
	error;
}

execute(gf, addr)
int *addr;
{
	register char *p1, *p2, c;

	if (gf) {
		if (circfl)
			return(0);
		p1 = linebuf;
		p2 = genbuf;
		while (*p1++ = *p2++);
		locs = p1 = loc2;
	} else {
		if (addr==zero)
			return(0);
		p1 = getline(*addr);
		locs = 0;
	}
	p2 = expbuf;
	if (circfl) {
		loc1 = p1;
		return(advance(p1, p2));
	}
	/* fast check for first character */
	if (*p2==CCHR) {
		c = p2[1];
		do {
			if (*p1!=c)
				continue;
			if (advance(p1, p2)) {
				loc1 = p1;
				return(1);
			}
		} while (*p1++);
		return(0);
	}
	/* regular algorithm */
	do {
		if (advance(p1, p2)) {
			loc1 = p1;
			return(1);
		}
	} while (*p1++);
	return(0);
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
		loc2 = lp;
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

	case CBRA:
		braslist[*ep++] = lp;
		continue;

	case CKET:
		braelist[*ep++] = lp;
		continue;

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
			if (lp==locs)
				break;
			if (advance(lp, ep))
				return(1);
		} while (lp &gt; curlp);
		return(0);

	default:
		error;
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

putd()
{
	register r;
	extern ldivr;

	count[1] = ldiv(count[0], count[1], 10);
	count[0] = 0;
	r = ldivr;
	if (count[1])
		putd();
	putchar(r + &#39;0&#39;);
}

puts(as)
{
	register char *sp;

	sp = as;
	col = 0;
	while (*sp)
		putchar(*sp++);
	putchar(&#39;\n&#39;);
}

char	line[70];
char	*linp	line;

putchar(ac)
{
	register char *lp;
	register c;

	lp = linp;
	c = ac;
	if (listf) {
		col++;
		if (col &gt;= 72) {
			col = 0;
			*lp++ = &#39;\\&#39;;
			*lp++ = &#39;\n&#39;;
		}
		if (c==&#39;\t&#39;) {
			c = &#39;&gt;&#39;;
			goto esc;
		}
		if (c==&#39;\b&#39;) {
			c = &#39;&lt;&#39;;
		esc:
			*lp++ = &#39;-&#39;;
			*lp++ = &#39;\b&#39;;
			*lp++ = c;
			goto out;
		}
		if (c&lt;&#39; &#39; &amp;&amp; c!= &#39;\n&#39;) {
			*lp++ = &#39;\\&#39;;
			*lp++ = (c&gt;&gt;3)+&#39;0&#39;;
			*lp++ = (c&amp;07)+&#39;0&#39;;
			col =+ 2;
			goto out;
		}
	}
	*lp++ = c;
out:
	if(c == &#39;\n&#39; || lp &gt;= &amp;line[64]) {
		linp = line;
		write(1, line, lp-line);
		return;
	}
	linp = lp;
}

/*
 * Get process ID routine if system call is unavailable.
getpid()
{
	register f;
	int b[1];

	f = open(&quot;/dev/kmem&quot;, 0);
	if(f &lt; 0)
		return(-1);
	seek(f, 0140074, 0);
	read(f, b, 2);
	seek(f, b[0]+8, 0);
	read(f, b, 2);
	close(f);
	return(b[0]);
}
 */
