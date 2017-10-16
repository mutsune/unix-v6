#

/*
 * incremental dump
 * dump fisbuodh filesystem
 * f take output tape from arglist
 * i from date in /etc/dtab
 * s specify tape size in feet (feet = blocks/9)
 * b specify tape size in blocks
 * u update /etc/dtab to current date
 * 0 dump from the epoch
 * d dump specified number of days
 * h dump specified number of hours
 * a on incremental dump, dump files even &gt;= MAXSIZE
 */

char	*dargv[]
{
	0,
	&quot;i&quot;,
	&quot;/dev/rp0&quot;,
	0
};

#include &quot;/usr/sys/ino.h&quot;
#include &quot;/usr/sys/filsys.h&quot;
#define	MAXSIZE	1000
struct filsys sblock;
struct
{
	char	name[16];
	int	date[2];
} dtab[10];
char	*dfile	&quot;/etc/dtab&quot;;
char	*ofile;
int	*talist;
int	fi;
int	buf[256];
int	dbuf[256];
int	ibuf[256];
int	vbuf[256];
char	*date[2];
char	*ddate[2];
int	fo	-1;
int	pher;
int	dflg;
int	iflg;
int	cflg;
int	aflg;
char	*tsize	19000;
char	*taddr;

main(argc, argv)
char **argv;
{
	char *key;
	int s, i, nfil, nblk, f;
	register *tap;
	register struct inode *ip;
	int ino;

	ofile = &quot;/dev/mt0&quot;;
	time(date);
	if(argc == 1) {
		argv = dargv;
		for(argc = 1; dargv[argc]; argc++);
	}

	argc--;
	argv++;
	key = *argv;
	while(*key)
	switch(*key++) {

	default:
		printf(&quot;bad character in key\n&quot;);
		exit();

	case &#39;a&#39;: /* dump all (even large) */
		aflg++;
		continue;

	case &#39;-&#39;:
		continue;

	case &#39;c&#39;: /* increment file name */
		cflg++;
		continue;

	case &#39;f&#39;: /* file name from arg list */
		argc--;
		argv++;
		ofile = *argv;
		continue;

	case &#39;i&#39;: /* date from date file */
		iflg++;
		continue;

	case &#39;s&#39;: /* tape size */
		tsize = number(argv[1]) * 9;
		argv++;
		argc--;
		continue;

	case &#39;b&#39;: /* tape size */
		tsize = number(argv[1]);
		argv++;
		argc--;
		continue;

	case &#39;u&#39;: /* rewrite date */
		dflg++;
		continue;

	case &#39;0&#39;: /* dump all */
		ddate[0] = ddate[1] = 0;
		continue;

	case &#39;d&#39;: /* dump some number of days */
		i = 21600;
		goto sd;

	case &#39;h&#39;: /* dump some number of hours */
		i = 900;
		goto sd;

	sd:
		ddate[0] = date[0];
		ddate[1] = date[1];
		s = number(argv[1])*4;
		argv++;
		argc--;
		while(s) {
			if(i &gt; ddate[1])
				ddate[0]--;
			ddate[1] =- i;
			s--;
		}
		continue;
	}
	if(argc &lt;= 1) {
		printf(&quot;no file system specified\n&quot;);
		exit();
	}
	if(iflg) {
		f = open(dfile, 0);
		if(f &gt;= 0) {
			read(f, dtab, sizeof dtab);
			close(f);
			for(i=0; i&lt;10; i++)
				if(equal(dtab[i].name, argv[1])) {
					ddate[0] = dtab[i].date[0];
					ddate[1] = dtab[i].date[1];
				}
		}
	}
	printf(&quot;%s:\n&quot;, argv[1]);
	fi = open(argv[1], 0);
	if(fi &lt; 0) {
		printf(&quot;cannot open %s\n&quot;, argv[1]);
		exit();
	}
	printf(&quot;incremental dump from\n&quot;);
	pdate(ddate);
	sync();
	bread(1, &amp;sblock);
	talist = sbrk(size(0, sblock.s_isize*32)*512);
	tap = talist;
	if(tap == -1) {
		printf(&quot;No memory\n&quot;);
		exit();
	}
	nfil = 0;
	nblk = size(0, sblock.s_isize*32);
	ino = 0;
	for(i=0; i&lt;sblock.s_isize; i++) {
		bread(i+2, buf);
		for(ip = &amp;buf[0]; ip &lt; &amp;buf[256]; ip++) {
			ino++;
			if(ip-&gt;i_mode == 0 || ip-&gt;i_nlink == 0) {
				*tap++ = -1;
				continue;
			}
			if(ip-&gt;i_mtime[0] &lt; ddate[0])
				goto no;
			if(ip-&gt;i_mtime[0] == ddate[0] &amp;&amp;
			   ip-&gt;i_mtime[1] &lt;  ddate[1])
				goto no;
			s = size(ip-&gt;i_size0&amp;0377, ip-&gt;i_size1) + 1;
			if (s&gt;MAXSIZE &amp;&amp; aflg==0 &amp;&amp; iflg!=0) {
				printf(&quot;%l big; not dumped.\n&quot;, ino);
				goto no;
			}
			nfil++;
			nblk =+ s;
			*tap++ = s;
			continue;
		no:
			*tap++ = 0;
		}
	}
	printf(&quot;%l files\n%l blocks\n&quot;, nfil, nblk);
	i = ldiv(0, nblk, ldiv(0, tsize, 10));
	printf(&quot;%l.%l tapes\n&quot;, i/10, i%10);
	tap = buf;
	clrbuf(tap);
	*tap++ = sblock.s_isize;
	*tap++ = sblock.s_fsize;
	*tap++ = date[0];
	*tap++ = date[1];
	*tap++ = ddate[0];
	*tap++ = ddate[1];
	*tap++ = tsize;
	swrite(buf);
	i = size(0, sblock.s_isize*32);
	tap = talist;
	while(i--) {
		bwrite(tap);
		tap =+ 256;
	}
	tap = talist;
	for(i=0; i&lt;sblock.s_isize; i++) {
		bread(i+2, buf);
		for(ip = &amp;buf[0]; ip &lt; &amp;buf[256]; ip++) {
			if(*tap &amp;&amp; *tap != -1)
				dump(ip, *tap-1);
			tap++;
		}
	}
	printf(&quot;%l phase errors\n&quot;, pher);
	if(!dflg)
		exit();
	for(i=0; i&lt;10; i++)
		dtab[i].name[0] = 0;
	f = open(dfile, 2);
	if(f &lt; 0) {
		f = creat(dfile, 0666);
		if(f &lt; 0) {
			printf(&quot;cannot create %s\n&quot;, dfile);
			exit();
		}
	} else
		read(f, dtab, sizeof dtab);
	for(i=0; i&lt;10; i++)
		if(dtab[i].name[0] == 0 || equal(dtab[i].name, argv[1]))
			goto found;
	printf(&quot;%s full\n&quot;, dfile);
	exit();

found:
	for(s=0; s&lt;15; s++) {
		dtab[i].name[s] = argv[1][s];
		if(argv[1][s] == 0)
			break;
	}
	dtab[i].date[0] = date[0];
	dtab[i].date[1] = date[1];
	seek(f, 0, 0);
	write(f, dtab, sizeof dtab);
	printf(&quot;date updated\n&quot;);
	pdate(date);
}

pdate(d)
int *d;
{

	if(d[0] == 0 &amp;&amp; d[1] == 0)
		printf(&quot;the epoch\n&quot;); else
		printf(ctime(d));
}

dump(ip, sz)
struct inode *ip;
{
	register *p, *q, *r;

	p = dbuf;
	q = ip;
	clrbuf(p);
	while(q &lt; &amp;ip-&gt;i_mtime[2])
		*p++ = *q++;
	swrite(dbuf);
	if(ip-&gt;i_mode &amp; (IFBLK&amp;IFCHR)) {
		if(sz != 0)
			printf(&quot;special\n&quot;);
		return;
	}
	for(p = &amp;ip-&gt;i_addr[0]; p &lt; &amp;ip-&gt;i_addr[8]; p++)
	if(*p) {
		if(ip-&gt;i_mode&amp;ILARG) {
			bread(*p, ibuf);
			for(q = &amp;ibuf[0]; q &lt; &amp;ibuf[256]; q++)
			if(*q) {
				if(p == &amp;ip-&gt;i_addr[7]) {
					bread(*q, vbuf);
					for(r = &amp;vbuf[0]; r &lt; &amp;vbuf[256]; r++)
					if(*r) {
						if(--sz &lt; 0)
							goto pe;
						bread(*r, dbuf);
						bwrite(dbuf);
					}
					continue;
				}
				if(--sz &lt; 0)
					goto pe;
				bread(*q, dbuf);
				bwrite(dbuf);
			}
		} else {
			if(--sz &lt; 0)
				goto pe;
			bread(*p, dbuf);
			bwrite(dbuf);
		}
	}
	if(sz)
		goto pe;
	return;

pe:
	clrbuf(dbuf);
	while(--sz &gt;= 0)
		bwrite(dbuf);
	pher++;
}

bread(bno, b)
{

	seek(fi, bno, 3);
	if(read(fi, b, 512) != 512) {
		printf(&quot;read error %l\n&quot;, bno);
	}
}

clrbuf(b)
int *b;
{
	register i, *p;

	p = b;
	i = 256;
	while(i--)
		*p++ = 0;
}

swrite(b)
int *b;
{
	register i, s, *p;

	i = 254;
	s = taddr;
	p = b;
	while(i--)
		s =+ *p++;
	*p++ = taddr;
	*p = 031415 - s;
	bwrite(b);
}

bwrite(b)
{

	if(taddr == 0) {
		if(fo != -1) {
			printf(&quot;change tapes\n&quot;);
			close(fo);
			rdline();
		}
		otape();
	}
wloop:
	if(write(fo, b, 512) != 512) {
		printf(&quot;write error\n&quot;);
		rdline();
		seek(fo, taddr, 3);
		goto wloop;
	}
	taddr++;
	if(taddr &gt;= tsize)
		taddr = 0;
}

rdline()
{
	int c;

loop:
	c = 0;
	read(0, &amp;c, 1);
	if(c == 0)
		exit();
	if(c != &#39;\n&#39;)
		goto loop;
}

number(s)
char *s;
{
	register n, c;

	n = 0;
	while(c = *s++) {
		if(c&lt;&#39;0&#39; || c&gt;&#39;9&#39;)
			continue;
		n = n*10+c-&#39;0&#39;;
	}
	return(n);
}

size(s0, s1)
{
	register s;
	extern ldivr;

	s = ldiv(s0&amp;0377, s1, 512);
	if(ldivr)
		s++;
	return(s);
}

otape()
{
	register char *p;

	fo = creat(ofile, 0666);
	if(fo &lt; 0) {
		printf(&quot;can not open %s\n&quot;, ofile);
		exit();
	}
	if(!cflg)
		return;
	p = ofile;
	while(*p++)
		;
	p[-2]++;
}

equal(a, b)
char *a, *b;
{

	while(*a++ == *b)
		if(*b++ == 0)
			return(1);
	return(0);
}
