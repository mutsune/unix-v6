#
/*
 *   print file with headings
 *  2+head+2+page[56]+5
 */

int	ncol	1;
char	*header;
int	col;
int	icol;
int	file;
char	*bufp;
#define	BUFS	5120
char	buffer[BUFS];
#define	FF	014
int	line;
char	*colp[72];
int	nofile;
char	isclosed[10];
int	peekc;
int	fpage;
int	page;
int	colw;
int	nspace;
int	width	72;
int	length	66;
int	plength 61;
int	margin	10;
int	ntflg;
int	mflg;
int	tabc;
char	*tty;
int	mode;

struct inode {
	int dev;
	int inum;
	int flags;
	char nlink;
	char uid;
	char gid;
	char siz0;
	int size;
	int ptr[8];
	int atime[2];
	int mtime[2];
};

main(argc, argv)
char **argv;
{
	int nfdone;
	int onintr();
	extern fout;

	tty = &quot;/dev/ttyx&quot;;
	fout = dup(1);
	close(1);
	if ((signal(2, 1) &amp; 01) == 0)
		signal(2, onintr);
	fixtty();
	for (nfdone=0; argc&gt;1; argc--) {
		argv++;
		if (**argv == &#39;-&#39;) {
			switch (*++*argv) {
			case &#39;h&#39;:
				if (argc&gt;=2) {
					header = *++argv;
					argc--;
				}
				continue;

			case &#39;t&#39;:
				ntflg++;
				continue;

			case &#39;l&#39;:
				length = getn(++*argv);
				continue;

			case &#39;w&#39;:
				width = getn(++*argv);
				continue;

			case &#39;s&#39;:
				if (*++*argv)
					tabc = **argv;
				else
					tabc = &#39;\t&#39;;
				continue;

			case &#39;m&#39;:
				mflg++;
				continue;

			default:
				ncol = getn(*argv);
				continue;
			}
		} else if (**argv == &#39;+&#39;) {
			fpage = getn(++*argv);
		} else {
			print(*argv, argv);
			nfdone++;
			if (mflg)
				break;
		}
	}
	if (nfdone==0)
		print(0);
	flush();
	onintr();
}

onintr()
{

	chmod(tty, mode);
	exit(0);
}

fixtty()
{
	struct inode sbuf;
	extern fout;

	tty[8] = ttyn(fout);
	fstat(fout, &amp;sbuf);
	mode = sbuf.flags&amp;0777;
	chmod(tty, 0600);
}

print(fp, argp)
char *fp;
char **argp;
{
	struct inode sbuf;
	register int sncol, sheader;
	register char *cbuf;
	extern fout;

	if (ntflg)
		margin = 0;
	else
		margin = 10;
	if (length &lt;= margin)
		length = 66;
	if (width &lt;= 0)
		width = 72;
	if (ncol&gt;72 || ncol&gt;width) {
		write(2, &quot;Very funny.\n&quot;, 12);
		exit();
	}
	if (mflg) {
		mopen(argp);
		ncol = nofile;
	}
	colw = width/ncol;
	sncol = ncol;
	sheader = header;
	plength = length-5;
	if (ntflg)
		plength = length;
	if (--ncol&lt;0)
		ncol = 0;
	if (mflg)
		fp = 0;
	if (fp) {
		file = open(fp, 0);
		if (file&lt;0)
			return;
		fstat(file, &amp;sbuf);
	} else {
		file = 0;
		time(sbuf.mtime);
	}
	if (header == 0)
		header = fp;
	cbuf = ctime(sbuf.mtime);
	cbuf[16] = &#39;\0&#39;;
	cbuf[24] = &#39;\0&#39;;
	page = 1;
	icol = 0;
	colp[ncol] = bufp = buffer;
	if (mflg==0)
		nexbuf();
	while (mflg&amp;&amp;nofile || (!mflg)&amp;&amp;tpgetc(ncol)&gt;0) {
		if (mflg==0) {
			colp[ncol]--;
			if (colp[ncol] &lt; buffer)
				colp[ncol] = &amp;buffer[BUFS];
		}
		line = 0;
		if (ntflg==0) {
			puts(&quot;\n\n&quot;);
			puts(cbuf+4);
			puts(&quot; &quot;);
			puts(cbuf+20);
			puts(&quot;  &quot;);
			puts(header);
			puts(&quot; Page &quot;);
			putd(page);
			puts(&quot;\n\n\n&quot;);
		}
		putpage();
		if (ntflg==0)
			while(line&lt;length)
				put(&#39;\n&#39;);
		page++;
	}
	if (file)
		close(file);
	ncol = sncol;
	header = sheader;
}

mopen(ap)
char **ap;
{
	register char **p, *p1;

	p = ap;
	while ((p1 = *p++) &amp;&amp; p1 != -1) {
		isclosed[nofile] = fopen(p1, &amp;buffer[2*259*nofile]);
		if (++nofile&gt;=10) {
			write(2, &quot;Too many args.\n&quot;, 15);
			exit();
		}
	}
}

putpage()
{
	register int lastcol, i, c;
	int j;

	if (ncol==0) {
		while (line&lt;plength) {
			while((c = tpgetc(0)) &amp;&amp; c!=&#39;\n&#39; &amp;&amp; c!=FF)
				putcp(c);
			putcp(&#39;\n&#39;);
			line++;
			if (c==FF)
				break;
		}
		return;
	}
	colp[0] = colp[ncol];
	if (mflg==0) for (i=1; i&lt;=ncol; i++) {
		colp[i] = colp[i-1];
		for (j = margin; j&lt;length; j++)
			while((c=tpgetc(i))!=&#39;\n&#39;)
				if (c==0)
					break;
	}
	while (line&lt;plength) {
		lastcol = colw;
		for (i=0; i&lt;ncol; i++) {
			while ((c=pgetc(i)) &amp;&amp; c!=&#39;\n&#39;)
				if (col&lt;lastcol || tabc!=0)
					put(c);
			if (c==0 &amp;&amp; ntflg)
				return;
			if (tabc)
				put(tabc);
			else while (col&lt;lastcol)
				put(&#39; &#39;);
			lastcol =+ colw;
		}
		while ((c = pgetc(ncol)) &amp;&amp; c!=&#39;\n&#39;)
			put(c);
		put(&#39;\n&#39;);
	}
}

nexbuf()
{
	register int n;
	register char *rbufp;

	rbufp = bufp;
	n = &amp;buffer[BUFS] - rbufp;
	if (n&gt;512)
		n = 512;
	if ((n = read(file, rbufp, n)) &lt;= 0)
		*rbufp = 0376;
	else {
		rbufp =+ n;
		if (rbufp &gt;= &amp;buffer[BUFS])
			rbufp = buffer;
		*rbufp = 0375;
	}
	bufp = rbufp;
}

tpgetc(ai)
{
	register char **p;
	register int c, i;

	i = ai;
	if (mflg) {
		if ((c = getc(&amp;buffer[2*259*i])) &lt; 0) {
			if (isclosed[i]==0) {
				isclosed[i] = 1;
				if (--nofile &lt;= 0)
					return(0);
			}
			return(&#39;\n&#39;);
		}
		if (c==FF &amp;&amp; ncol&gt;0)
			c = &#39;\n&#39;;
		return(c);
	}
loop:
	c = **(p = &amp;colp[i]) &amp; 0377;
	if (c == 0375) {
		nexbuf();
		c = **p &amp; 0377;
	}
	if (c == 0376)
		return(0);
	(*p)++;
	if (*p &gt;= &amp;buffer[BUFS])
		*p = buffer;
	if (c==0)
		goto loop;
	return(c);
}

pgetc(i)
{
	register int c;

	if (peekc) {
		c = peekc;
		peekc = 0;
	} else
		c = tpgetc(i);
	if (tabc)
		return(c);
	switch (c) {

	case &#39;\t&#39;:
		icol++;
		if ((icol&amp;07) != 0)
			peekc = &#39;\t&#39;;
		return(&#39; &#39;);

	case &#39;\n&#39;:
		icol = 0;
		break;

	case 010:
	case 033:
		icol--;
		break;
	}
	if (c &gt;= &#39; &#39;)
		icol++;
	return(c);
}

puts(as)
char *as;
{
	register int c;
	register char *s;

	if ((s=as)==0)
		return;
	while (c = *s++)
		put(c);
}

putd(an)
{
	register int a, n;

	n = an;
	if (a = n/10)
		putd(a);
	put(n%10 + &#39;0&#39;);
}

put(ac)
{
	register int ns, c;

	c = ac;
	if (tabc) {
		putcp(c);
		if (c==&#39;\n&#39;)
			line++;
		return;
	}
	switch (c) {

	case &#39; &#39;:
		nspace++;
		col++;
		return;

	case &#39;\n&#39;:
		col = 0;
		nspace = 0;
		line++;
		break;

	case 010:
	case 033:
		if (--col&lt;0)
			col = 0;
		if (--nspace&lt;0)
			nspace = 0;

	}
	while(nspace) {
		if (nspace&gt;2 &amp;&amp; col &gt; (ns=((col-nspace)|07))) {
			nspace = col-ns-1;
			putcp(&#39;\t&#39;);
		} else {
			nspace--;
			putcp(&#39; &#39;);
		}
	}
	if (c &gt;= &#39; &#39;)
		col++;
	putcp(c);
}

getn(ap)
char *ap;
{
	register int n, c;
	register char *p;

	p = ap;
	n = 0;
	while ((c = *p++) &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;)
		n = n*10 + c - &#39;0&#39;;
	return(n);
}

putcp(c)
{
	if (page &gt;= fpage)
		putchar(c);
}
