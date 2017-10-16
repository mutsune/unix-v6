#

char	*dargv[]
{
	&quot;/dev/rrk2&quot;,
	&quot;/dev/rrp0&quot;,
	0
};
#define	NINODE	16*16
#define	NB	10
#include &quot;/usr/sys/ino.h&quot;
#include &quot;/usr/sys/filsys.h&quot;

struct	inode	inode[NINODE];
struct	filsys	sblock;

int	sflg;

int	fi;
int	nifiles;
int	nfile;
int	nspcl;
int	nlarg;
int	nvlarg;
int	nindir;
int	nvindir;
int	ndir;
int	nused;
int	nfree;
int	ino;
int	ndup;
int	blist[10] { -1};
int	nerror;
int	bmap[4096];

main(argc, argv)
char **argv;
{
	register char **p;
	register int n, *lp;

	if (argc == 1) {
		for (p = dargv; *p;)
			check(*p++);
		return(nerror);
	}
	while (--argc) {
		argv++;
		if (**argv==&#39;-&#39;) switch ((*argv)[1]) {
		case &#39;s&#39;:
			sflg++;
			continue;

		case &#39;b&#39;:
			lp = blist;
			while (lp &lt; &amp;blist[NB-1] &amp;&amp; (n = number(argv[1]))) {
				*lp++ = n;
				argv++;
				argc--;
			}
			*lp++ = -1;
			continue;

		default:
			printf(&quot;Bad flag\n&quot;);
		}
		check(*argv);
	}
	return(nerror);
}

check(file)
char *file;
{
	register *ip, i, j;

	fi = open(file, sflg?2:0);
	if (fi &lt; 0) {
		printf(&quot;cannot open %s\n&quot;, file);
		nerror =| 04;
		return;
	}
	printf(&quot;%s:\n&quot;, file);
	nfile = 0;
	nspcl = 0;
	nlarg = 0;
	nvlarg = 0;
	nindir = 0;
	nvindir = 0;
	ndir = 0;
	nused = 0;
	nfree = 0;
	ndup = 0;
	for (ip = bmap; ip &lt; &amp;bmap[4096];)
		*ip++ = 0;
	sync();
	bread(1, &amp;sblock, 512);
	nifiles = sblock.s_isize*16;
	for(i=0; ino &lt; nifiles; i =+ NINODE/16) {
		bread(i+2, inode, sizeof inode);
		for(j=0; j&lt;NINODE &amp;&amp; ino&lt;nifiles; j++) {
			ino++;
			pass1(&amp;inode[j]);
		}
	}
	ino = 0;
	sync();
	bread(1, &amp;sblock, 512);
	if (sflg) {
		makefree();
		return;
	}
	while(i = alloc()) {
		if (chk(i, &quot;free&quot;))
			break;
		nfree++;
	}
	if (ndup) {
		printf(&quot;%l dups in free\n&quot;, ndup);
		nerror =| 02;
	}
	j = 0;
	for (ip = bmap; ip &lt; &amp;bmap[4096];) {
		i = *ip++;
		while (i) {
			if (i&lt;0)
				j--;
			i =&lt;&lt; 1;
		}
	}
	j =+ sblock.s_fsize - sblock.s_isize - 2;
	if (j)
		printf(&quot;missing%5l\n&quot;, j);
	printf(&quot;spcl  %6l\n&quot;, nspcl);
	printf(&quot;files %6l\n&quot;, nfile);
	printf(&quot;large %6l\n&quot;, nlarg);
	if (nvlarg)
		printf(&quot;huge  %6l\n&quot;, nvlarg);
	printf(&quot;direc %6l\n&quot;, ndir);
	printf(&quot;indir %6l\n&quot;, nindir);
	if (nvindir)
		printf(&quot;indir2%6l\n&quot;, nvindir);
	printf(&quot;used  %6l\n&quot;, nused);
	printf(&quot;free  %6l\n&quot;, nfree);
	close(fi);
}

pass1(aip)
struct inode *aip;
{
	int buf[256], vbuf[256];
	register i, j, *ip;

	ip = aip;
	if ((ip-&gt;i_mode&amp;IALLOC) == 0)
		return;
	if ((ip-&gt;i_mode&amp;IFCHR&amp;IFBLK) != 0) {
		nspcl++;
		return;
	}
	if ((ip-&gt;i_mode&amp;IFMT) == IFDIR)
		ndir++;
	else
		nfile++;
	if ((ip-&gt;i_mode&amp;ILARG) != 0) {
		nlarg++;
		for(i=0; i&lt;7; i++)
		if (ip-&gt;i_addr[i] != 0) {
			nindir++;
			if (chk(ip-&gt;i_addr[i], &quot;indirect&quot;))
				continue;
			bread(ip-&gt;i_addr[i], buf, 512);
			for(j=0; j&lt;256; j++)
			if (buf[j] != 0)
				chk(buf[j], &quot;data (large)&quot;);
		}
		if (ip-&gt;i_addr[7]) {
			nvlarg++;
			if (chk(ip-&gt;i_addr[7], &quot;indirect&quot;))
				return;
			bread(ip-&gt;i_addr[7], buf, 512);
			for(i=0; i&lt;256; i++)
			if (buf[i] != 0) {
				nvindir++;
				if (chk(buf[i], &quot;2nd indirect&quot;))
					continue;
				bread(buf[i], vbuf, 512);
				for(j=0; j&lt;256; j++)
				if (vbuf[j])
					chk(vbuf[j], &quot;data (very large)&quot;);
			}
		}
		return;
	}
	for(i=0; i&lt;8; i++) {
		if (ip-&gt;i_addr[i] != 0)
			chk(ip-&gt;i_addr[i], &quot;data (small)&quot;);
	}
}

chk(ab, s)
char *ab;
{
	register char *b;
	register n, m;

	b = ab;
	if (ino)
		nused++;
	if (b&lt;sblock.s_isize+2 || b&gt;=sblock.s_fsize) {
		printf(&quot;%l bad; inode=%l, class=%s\n&quot;, b, ino, s);
		return(1);
	}
	m = 1 &lt;&lt; (b&amp;017);
	n = (b&gt;&gt;4) &amp; 07777;
	if (bmap[n]&amp;m) {
		printf(&quot;%l dup; inode=%l, class=%s\n&quot;, b, ino, s);
		ndup++;
	}
	bmap[n] =| m;
	for (n=0; blist[n] != -1; n++)
		if (b == blist[n])
			printf(&quot;%l arg; inode=%l, class=%s\n&quot;, b, ino, s);
	return(0);
}

alloc()
{
	register b, i;
	int buf[256];

	i = --sblock.s_nfree;
	if (i&lt;0 || i&gt;=100) {
		printf(&quot;bad freeblock\n&quot;);
		return(0);
	}
	b = sblock.s_free[i];
	if (b == 0)
		return(0);
	if (sblock.s_nfree &lt;= 0) {
		bread(b, buf, 512);
		sblock.s_nfree = buf[0];
		for(i=0; i&lt;100; i++)
			sblock.s_free[i] = buf[i+1];
	}
	return(b);
}

bread(bno, buf, cnt)
int *buf;
{
	register *ip;

	seek(fi, bno, 3);
	if (read(fi, buf, cnt) != cnt) {
		printf(&quot;read error %d\n&quot;, bno);
		if (sflg) {
			printf(&quot;No update\n&quot;);
			sflg = 0;
		}
		for (ip = buf; ip &lt; &amp;buf[256];)
			*ip++ = 0;
	}
}

free(in)
{
	register i;
	int buf[256];

	if (sblock.s_nfree &gt;= 100) {
		buf[0] = sblock.s_nfree;
		for(i=0; i&lt;100; i++)
			buf[i+1] = sblock.s_free[i];
		sblock.s_nfree = 0;
		bwrite(in, buf);
	}
	sblock.s_free[sblock.s_nfree++] = in;
}

bwrite(bno, buf)
{

	seek(fi, bno, 3);
	if (write(fi, buf, 512) != 512)
		printf(&quot;write error %d\n&quot;, bno);
}

number(as)
char *as;
{
	register n, c;
	register char *s;

	s = as;
	n = 0;
	while ((c = *s++) &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;) {
		n = n*10+c-&#39;0&#39;;
	}
	return(n);
}

makefree()
{
	register i;

	sblock.s_nfree = 0;
	sblock.s_ninode = 0;
	sblock.s_flock = 0;
	sblock.s_ilock = 0;
	sblock.s_fmod = 0;
	free(0);
	for(i=sblock.s_fsize-1; i&gt;=sblock.s_isize+2; i--) {
		if ((bmap[(i&gt;&gt;4)&amp;07777] &amp; (1&lt;&lt;(i&amp;017)))==0)
			free(i);
	}
	bwrite(1, &amp;sblock);
	close(fi);
	sync();
	return;
}
