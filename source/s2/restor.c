#

/*
 * restore from incremental dumps
 */

char	*dargv[]
{
	0,
	&quot;t&quot;,
	0
};
char	*ifile;
char	*ofile;
#include &quot;/usr/sys/ino.h&quot;
#include &quot;/usr/sys/filsys.h&quot;

struct	filsys	sblock;
int	isize;
int	*talist;
int	fi;
int	buf[256];
int	dbuf[256];
int	cbuf[256];
char	*date[2];
char	*ddate[2];
int	fo;
int	pher;
char	*tsize	15000;
int	iflg;
int	wflg;
int	cflg;
char	file[10];
int	ilist[100];

main(argc, argv)
char **argv;
{
	char *key;
	register *tap, *p;
	register struct inode *ip;
	int i, com, sz, *q, l;

	ifile = &quot;/dev/mt0&quot;;
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

	case &#39;t&#39;:
	case &#39;r&#39;:
	case &#39;x&#39;:
		com = key[-1];
		continue;

	case &#39;i&#39;:
		iflg++;
		continue;

	case &#39;-&#39;:
		continue;

	case &#39;c&#39;:
		cflg++;
		continue;

	case &#39;f&#39;:
		argv++;
		argc--;
		ifile = *argv;
		continue;

	case &#39;w&#39;:
		wflg++;
		continue;

	}
	otape();
	sread(buf, 0);
	tap = buf;
	isize = *tap++;
	*tap++;		/* fsize */
	date[0] = *tap++;
	date[1] = *tap++;
	ddate[0] = *tap++;
	ddate[1] = *tap++;
	tsize = *tap++;
	i = size(0, isize*32);
	talist = sbrk(i*512);
	tap = talist;
	while(i--) {
		tread(tap, 0);
		tap =+ 256;
	}
	switch(com) {

case &#39;t&#39;:
	l = 0;
	com = 0;
	pdate(ddate);
	pdate(date);
	tap = talist;
	for(i=0; i&lt;isize*16; i++) {
		sz = *tap++;
		if(sz == 0 || sz == -1) {
			if(com == 0)
				continue;
			if(i == com) {
				printf(&quot;%l&quot;, i);
				l =+ 5;
			} else {
				printf(&quot;%l-%l&quot;, com, i);
				l =+ 10;
			}
			if(l &gt; 60) {
				printf(&quot;\n&quot;);
				l = 0;
			} else
				printf(&quot;,&quot;);
			com = 0;
		} else
		if(com == 0)
			com = i+1;
	}
	if(com)
		printf(&quot;%l-\n&quot;, com);
	printf(&quot;\n&quot;);
	exit();

case &#39;r&#39;:
	if(argc &lt;= 1) {
		printf(&quot;no filesystem name\n&quot;);
		exit();
	}
	ofile = argv[1];
	fo = open(ofile, 2);
	if(fo &lt; 0) {
		printf(&quot;can not open %s\n&quot;, ofile);
		exit();
	}
	printf(&quot;last chance before scribbling on %s\n&quot;, ofile);
	getchar();
	dread(1, &amp;sblock);
	tap = talist;
	for(i=0; i&lt;sblock.s_isize; i++) {
		if(i &gt;= isize)
			break;
		dread(i+2, buf);
		for(ip = &amp;buf[0]; ip &lt; &amp;buf[256]; ip++) {
			sz = *tap++;
			if(sz == 0)
				continue;
			dealoc(ip);
			if(sz == -1) {
				for(p = ip; p &lt; &amp;ip-&gt;i_mtime[2]; )
					*p++ = 0;
				continue;
			}
			sread(dbuf, 0);
			q = dbuf;
			for(p = ip; p &lt; &amp;ip-&gt;i_mtime[2]; )
				*p++ = *q++;
			restor(ip, sz-1);
		}
		dwrite(i+2, buf);
	}
	dwrite(1, &amp;sblock);
	com = 0;
	for(; i &lt; isize; i++)
		for(l = 0; l &lt; 16; l++) {
			sz = *tap++;
			if(sz != 0 &amp;&amp; sz != -1)
				com++;
		}
	if(com)
		printf(&quot;%l files not restored - small ilist\n&quot;, com);
	exit();

case &#39;x&#39;:
	i = 0;
	tap = ilist;
	while(argc &gt; 1) {
		i++;
		sz = number(argv[1]);
		argv++;
		argc--;
		if(sz &lt;= 0 || sz &gt;=isize*16) {
			printf(&quot;%l not in range\n&quot;, sz);
			continue;
		}
		if(talist[sz-1] == 0) {
			printf(&quot;%l not dumped\n&quot;, sz);
			continue;
		}
		if(talist[sz-1] == -1) {
			printf(&quot;%l does not exist\n&quot;, sz);
			continue;
		}
		*tap++ = sz;
	}
	if(i != 0 &amp;&amp; ilist[0] == 0)
		exit();
	tap = talist;
	for(i=1; i&lt;=isize*16; i++) {
		if(ilist[0] != 0) {
			for(sz=0; ilist[sz]; sz++)
				if(ilist[sz] == i)
					goto yes;
			sz = *tap++;
		no:
			if(sz == -1)
				sz = 0;
			while(sz--)
				tread(dbuf, 1);
			continue;
		}
	yes:
		sz = *tap++;
		if(sz == 0 || sz == -1)
			continue;
		fo = dwait(i);
		if(fo &lt; 0)
			goto no;
		sz--;
		sread(buf, 0);
		ip = buf;
		while(sz--) {
			tread(dbuf, 0);
			com = 512;
			if(ip-&gt;i_size0 == 0 &amp;&amp; ip-&gt;i_size1 &lt; 512)
				com = ip-&gt;i_size1;
			write(fo, dbuf, com);
			if(com &gt; ip-&gt;i_size1)
				ip-&gt;i_size0--;
			ip-&gt;i_size1 =- com;
		}
		close(fo);
		chmod(file, ip-&gt;i_mode);
		chown(file, ip-&gt;i_uid);
	}
	exit();

	}
}

dealoc(p)
struct inode *p;
{
	register struct inode *ip;
	register i, j;
	int k;
	int xbuf[256], ybuf[256];

	ip = p;
	if(ip-&gt;i_mode &amp; (IFCHR&amp;IFBLK))
		return;
	for(i=7; i&gt;=0; i--)
	if(ip-&gt;i_addr[i]) {
		if(ip-&gt;i_mode&amp;ILARG) {
			dread(ip-&gt;i_addr[i], xbuf);
			for(j=255; j&gt;=0; j--)
			if(xbuf[j]) {
				if(i == 7) {
					dread(xbuf[j], ybuf);
					for(k=255; k&gt;=0; k--)
					if(ybuf[k])
						free(ybuf[k]);
				}
				free(xbuf[j]);
			}
		}
		free(ip-&gt;i_addr[i]);
	}
}

restor(p, sz)
struct inode *p;
{
	register struct inode *ip;
	register i, j;
	int xbuf[256];

	ip = p;
	if(ip-&gt;i_mode &amp; (IFCHR&amp;IFBLK))
		return;
	for(i=0; i&lt;8; i++)
		ip-&gt;i_addr[i] = 0;
	if(sz &lt;= 8) {
		for(i=0; i&lt;sz; i++)
			ip-&gt;i_addr[i] = rcop();
		ip-&gt;i_mode =&amp; ~ILARG;
		return;
	}
	for(i=0; i&lt;256; i++)
		xbuf[i] = 0;
	for(j=0; sz &gt;= 256; j++) {
		if(j &lt;= 7)
			ip-&gt;i_addr[j] = alloc();
		if(j &gt;= 7)
			xbuf[j-7] = alloc();
		for(i=0; i&lt;256; i++)
			dbuf[i] = rcop();
		if(j &lt; 7)
			dwrite(ip-&gt;i_addr[j], dbuf); else
			dwrite(xbuf[j-7], dbuf);
		sz =- 256;
	}
	if(sz) {
		if(j &lt;= 7)
			ip-&gt;i_addr[j] = alloc();
		if(j &gt;= 7)
			xbuf[j-7] = alloc();
		for(i=0; i&lt;sz; i++)
			dbuf[i] = rcop();
		for(; i&lt;256; i++)
			dbuf[i] = 0;
		if(j &lt; 7)
			dwrite(ip-&gt;i_addr[j], dbuf); else
			dwrite(xbuf[j-7], dbuf);
	}
	if(j &gt;= 7)
		dwrite(ip-&gt;i_addr[7], xbuf);
	ip-&gt;i_mode =| ILARG;
}

rcop()
{
	register b;

	b = alloc();
	tread(cbuf, 0);
	dwrite(b, cbuf);
	return(b);
}

pdate(d)
int *d;
{

	if(d[0] == 0 &amp;&amp; d[1] == 0)
		printf(&quot;the epoch\n&quot;); else
		printf(ctime(d));
}

dread(bno, b)
{

	seek(fo, bno, 3);
	if(read(fo, b, 512) != 512) {
		printf(&quot;disk read error %l\n&quot;, bno);
		exit();
	}
}

dwrite(bno, b)
{

	seek(fo, bno, 3);
	if(write(fo, b, 512) != 512) {
		printf(&quot;disk write error %l\n&quot;, bno);
		exit();
	}
}

sread(b, flag)
int *b;
{
	register i, s, *p;

	tread(b, flag);
	if(flag)
		return;
	i = 256;
	s = 0;
	p = b;
	while(i--)
		s =+ *p++;
	if(s != 031415) {
		printf(&quot;checksum error\n&quot;);
		if(!iflg)
			exit();
	}
}

tread(b, flag)
int *b;
{
	register c;
	static char *pta, *ata, ctflg;

	if(pta++ &gt;= tsize) {
		pta = 1;
		ata = 0;
		close(fi);
		otape();
		ctflg++;
	}
	if(flag)
		return;
	if(ctflg) {
		printf(&quot;change tapes\n&quot;);
		if(ctflg &gt; 1)
			printf(&quot;skip %d tapes\n&quot;, ctflg-1);
		while((c = getchar()) != &#39;\n&#39;)
			if(c == 0)
				exit();
		ctflg = 0;
	}
	ata++;
	if(iflg)
	for(; pta != ata; ata++)
		read(fi, b, 512);
	if(pta != ata) {
		seek(fi, pta-ata, 4);
		ata = pta;
	}
	if(read(fi, b, 512) != 512) {
		printf(&quot;tape read error %l\n&quot;, ata-1);
		if(!iflg)
			exit();
		for(c=0; c&lt;256; c++)
			b[c] = 0;
	}
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

	fi = open(ifile, 0);
	if(fi &lt; 0) {
		printf(&quot;can not open %s\n&quot;, ifile);
		exit();
	}
	if(!cflg)
		return;
	p = ifile;
	while(*p++)
		;
	p[-2]++;
}

dwait(ino)
{
	register i;

	dconv(ino, file);
loop:
	if(wflg) {
		printf(&quot;%s &quot;, file);
		i = getchar();
		if(i == &#39;x&#39;)
			exit();
		if(i == &#39;\n&#39;)
			return(-1);
		if(i != &#39;y&#39;)
			goto flush;
		i = getchar();
		if(i != &#39;\n&#39;) {
		flush:
			while((i=getchar()) != &#39;\n&#39;)
				if(i == &#39;\0&#39;)
					exit();
			goto loop;
		}
	}
	i = creat(file, 0666);
	return(i);
}

dconv(n, p)
char *p;
{
	register i;

	if(i = ldiv(0, n, 10))
		p = dconv(i, p);
	*p++ = lrem(0, n, 10) + &#39;0&#39;;
	*p = &#39;\0&#39;;
	return(p);
}

alloc()
{
	register b, i;

	i = --sblock.s_nfree;
	if(i&lt;0 || i&gt;=100) {
		printf(&quot;bad freeblock\n&quot;);
		exit();
	}
	b = sblock.s_free[i];
	if(b == 0) {
		printf(&quot;out of freelist\n&quot;);
		exit();
	}
	if(sblock.s_nfree &lt;= 0) {
		dread(b, cbuf);
		sblock.s_nfree = cbuf[0];
		for(i=0; i&lt;100; i++)
			sblock.s_free[i] = cbuf[i+1];
	}
	return(b);
}

free(in)
{
	register i;

	if(sblock.s_nfree &gt;= 100) {
		cbuf[0] = sblock.s_nfree;
		for(i=0; i&lt;100; i++)
			cbuf[i+1] = sblock.s_free[i];
		sblock.s_nfree = 0;
		dwrite(in, cbuf);
	}
	sblock.s_free[sblock.s_nfree++] = in;
}
