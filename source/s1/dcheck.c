#

char	*dargv[]
{
	&quot;/dev/rrk2&quot;,
	&quot;/dev/rrp0&quot;,
	0
};

#define NINODE	16*16
#define	NI	20

#include &quot;/usr/sys/ino.h&quot;
#include &quot;/usr/sys/filsys.h&quot;

struct	inode	inode[NINODE];
struct	filsys	sblock;

int	sflg;
int	headpr;

int	ilist[NI] { -1};
int	fi;
char	*ecount;
char	*lasts;
int	ino;
int	nerror;
int	nfiles;
struct dir {
	int	ino;
	char	name[14];
};

main(argc, argv)
char **argv;
{
	register char **p;
	register int n, *lp;

	ecount = sbrk(0);
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

		case &#39;i&#39;:
			lp = ilist;
			while (lp &lt; &amp;ilist[NI-1] &amp;&amp; (n = number(argv[1]))) {
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
	register i, j;
	fi = open(file, 0);
	if(fi &lt; 0) {
		printf(&quot;cannot open %s\n&quot;, file);
		return;
	}
	headpr = 0;
	printf(&quot;%s:\n&quot;, file);
	sync();
	bread(1, &amp;sblock, 512);
	nfiles = sblock.s_isize*16;
	if (lasts &lt; nfiles) {
		if ((sbrk(nfiles - lasts)) == -1) {
			printf(&quot;Not enough core\n&quot;);
			exit(04);
		}
		lasts = nfiles;
	}
	for (i=0; i&lt;nfiles; i++)
		ecount[i] = 0;
	for(i=0; ino&lt;nfiles; i =+ NINODE/16) {
		bread(i+2, inode, sizeof inode);
		for(j=0; j&lt;NINODE &amp;&amp; ino&lt;nfiles; j++) {
			ino++;
			pass1(&amp;inode[j]);
		}
	}
	ino = 0;
	for (i=0; ino&lt;nfiles; i =+ NINODE/16) {
		bread(i+2, inode, sizeof inode);
		for (j=0; j&lt;NINODE &amp;&amp; ino&lt;nfiles; j++) {
			ino++;
			pass2(&amp;inode[j]);
		}
	}
}

pass1(aip)
struct inode *aip;
{
	register doff;
	register struct inode *ip;
	register struct dir *dp;
	int i;

	ip = aip;
	if((ip-&gt;i_mode&amp;IALLOC) == 0)
		return;
	if((ip-&gt;i_mode&amp;IFMT) != IFDIR)
		return;
	doff = 0;
	while (dp = dread(ip, doff)) {
		doff =+ 16;
		if (dp-&gt;ino==0)
			continue;
		for (i=0; ilist[i] != -1; i++)
			if (ilist[i]==dp-&gt;ino)
				printf(&quot;%5l arg; %l/%.14s\n&quot;, dp-&gt;ino, ino, dp-&gt;name);
		ecount[dp-&gt;ino]++;
	}
}

pass2(aip)
{
	register struct inode *ip;
	register i;

	ip = aip;
	i = ino;
	if ((ip-&gt;i_mode&amp;IALLOC)==0 &amp;&amp; ecount[i]==0)
		return;
	if (ip-&gt;i_nlink==ecount[i] &amp;&amp; ip-&gt;i_nlink!=0)
		return;
	if (headpr==0) {
		printf(&quot;entries	link cnt\n&quot;);
		headpr++;
	}
	printf(&quot;%l	%d	%d\n&quot;, ino,
	    ecount[i]&amp;0377, ip-&gt;i_nlink&amp;0377);
}

dread(aip, aoff)
{
	register b, off;
	register struct inode *ip;
	static ibuf[256];
	static char buf[512];

	off = aoff;
	ip = aip;
	if ((off&amp;0777)==0) {
		if (off==0177000) {
			printf(&quot;Monstrous directory %l\n&quot;, ino);
			return(0);
		}
		if ((ip-&gt;i_mode&amp;ILARG)==0) {
			if (off&gt;=010000 || (b = ip-&gt;i_addr[off&gt;&gt;9])==0)
				return(0);
			bread(b, buf, 512);
		} else {
			if (off==0) {
				if (ip-&gt;i_addr[0]==0)
					return(0);
				bread(ip-&gt;i_addr[0], ibuf, 512);
			}
			if ((b = ibuf[(off&gt;&gt;9)&amp;0177])==0)
				return(0);
			bread(b, buf, 512);
		}
	}
	return(&amp;buf[off&amp;0777]);
}

bread(bno, buf, cnt)
{

	seek(fi, bno, 3);
	if(read(fi, buf, cnt) != cnt) {
		printf(&quot;read error %d\n&quot;, bno);
		exit();
	}
}

bwrite(bno, buf)
{

	seek(fi, bno, 3);
	if(write(fi, buf, 512) != 512) {
		printf(&quot;write error %d\n&quot;, bno);
		exit();
	}
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
