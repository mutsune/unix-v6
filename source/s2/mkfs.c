#
struct
{
	char	*s_isize;
	char	*s_fsize;
	int	s_nfree;
	int	s_free[100];
	int	s_ninode;
	int	s_inode[100];
	char	s_flock;
	char	s_ilock;
	char	s_fmod;
	int	s_time[2];
} filsys;

struct inode
{
	int	i_number;
	int	i_mode;
	char	i_nlink;
	char	i_uid;
	char	i_gid;
	char	i_size0;
	char	*i_size1;
	int	i_addr[8];
	int	i_time[4];
};

/* modes */
#define	IALLOC	0100000
#define	IFMT	060000
#define		IFDIR	040000
#define		IFCHR	020000
#define		IFBLK	060000
#define	ILARG	010000
#define	ISUID	04000
#define	ISGID	02000
#define	IREAD	0400
#define	IWRITE	0200
#define	IEXEC	0100

int	utime[2];
int	fin;
int	fsi;
int	fso;
char	*charp;
int	buf[256];
char	string[50];
char	*fsys;
char	*proto;
int	f_n	1;
int	f_m	1;

main(argc, argv)
char **argv;
{
	int f, n;

	/*
	 * open relevent files
	 */

	time(utime);
	if(argc != 3) {
		printf(&quot;arg count\n&quot;);
		exit();
	}
	fsys = argv[1];
	for(n=0; f=fsys[n+1]; n++)
	if(fsys[n] == &#39;r&#39;) {
		if(f == &#39;k&#39;) {
			f_n = 24;
			f_m = 3;
		}
		if(f == &#39;p&#39;) {
			f_n = 10;
			f_m = 4;
		}
	}
	proto = argv[2];
	fso = creat(fsys, 0666);
	if(fso &lt; 0) {
		printf(&quot;%s: cannot create\n&quot;, fsys);
		exit();
	}
	fsi = open(fsys, 0);
	if(fsi &lt; 0) {
		printf(&quot;%s: cannot open\n&quot;, fsys);
		exit();
	}
	fin = open(proto, 0);
	if(fin &lt; 0) {
		n = 0;
		for(f=0; proto[f]; f++) {
			if(proto[f]&lt;&#39;0&#39; || proto[f]&gt;&#39;9&#39;) {
				printf(&quot;%s: cannot open\n&quot;, proto);
				exit();
			}
			n = n*10 + proto[f]-&#39;0&#39;;
		}
		filsys.s_fsize = n;
		filsys.s_isize = ldiv(0, n, 43+ldiv(0, n, 1000));
		printf(&quot;isize = %d\n&quot;, filsys.s_isize);
		if(f_n != 1)
			printf(&quot;free list %d/%d\n&quot;, f_m, f_n);
		charp = &quot;d--777 0 0 $ &quot;;
		goto f3;
	}

	/*
	 * get name of boot load program
	 * and read onto block 0
	 */

	getstr();
	f = open(string, 0);
	if(f &lt; 0) {
		printf(&quot;%s: cannot  open init\n&quot;, string);
		goto f2;
	}
	read(f, buf, 020);
	if(buf[0] != 0407) {
		printf(&quot;%s: bad format\n&quot;, string);
		goto f1;
	}
	n = buf[1]+buf[2];
	if(n &gt; 512) {
		printf(&quot;%s: too big\n&quot;, string);
		goto f1;
	}
	read(f, buf, n);
	wtfs(0, buf);

f1:
	close(f);

	/*
	 * get total disk size
	 * and inode block size
	 */

f2:
	filsys.s_fsize = getnum();
	filsys.s_isize = getnum();

f3:
	if(filsys.s_isize &gt; filsys.s_fsize ||
	   filsys.s_fsize-filsys.s_isize-2 &lt; filsys.s_isize) {
		printf(&quot;%l/%l: bad ratio\n&quot;, filsys.s_fsize, filsys.s_isize);
		exit();
	}
	bflist();

	/*
	 * initialize files
	 */

	for(n=0; n&lt;256; n++)
		buf[n] = 0;
	for(n=0; n!=filsys.s_isize; n++)
		wtfs(n+2, buf);
	cfile(0);

	/*
	 * write out super block
	 */

	for(n=0; n&lt;256; n++)
		buf[n] = 0;
	filsys.s_time[0] = utime[0];
	filsys.s_time[1] = utime[1];
	wtfs(1, &amp;filsys);
}

cfile(par)
struct inode *par;
{
	struct inode in;
	int db[256], ib[256];
	int dbc, ibc;
	static ino;
	int i, f, *p1, *p2;

	/*
	 * get mode, uid and gid
	 */

	getstr();
	in.i_mode = IALLOC;
	in.i_mode =| gmode(string[0], &quot;bcd&quot;, IFBLK, IFCHR, IFDIR);
	in.i_mode =| gmode(string[1], &quot;u&quot;, ISUID);
	in.i_mode =| gmode(string[2], &quot;g&quot;, ISGID);
	for(i=3; i&lt;6; i++) {
		if(string[i]&lt;&#39;0&#39; || string[i]&gt;&#39;7&#39;) {
			printf(&quot;%c/%s: bad digit\n&quot;, string[i], string);
			exit();
		}
		in.i_mode =| (string[i]-&#39;0&#39;)&lt;&lt;(15-3*i);
	}
	in.i_uid = getnum();
	in.i_gid = getnum();

	/*
	 * general initialization prior to
	 * switching on format
	 */

	ino++;
	in.i_number = ino;
	if(ldiv(0, ino, 16) &gt; filsys.s_isize) {
		printf(&quot;too many inodes\n&quot;);
		exit();
	}
	in.i_nlink = 1;
	in.i_size0 = 0;
	in.i_size1 = 0;
	for(i=0; i&lt;8; i++)
		in.i_addr[i] = 0;
	for(i=0; i&lt;256; i++) {
		db[i] = 0;
		ib[i] = 0;
	}
	if(par == 0) {
		par = &amp;in;
		in.i_nlink--;
	}
	dbc = 0;
	ibc = 0;
	switch(in.i_mode&amp;IFMT) {

	case 0:
		/*
		 * regular file
		 * contents is a file name
		 */

		getstr();
		f = open(string, 0);
		if(f &lt; 0) {
			printf(&quot;%s: cannot open\n&quot;, string);
			break;
		}
		while((i=read(f, db, 512)) &gt; 0) {
			in.i_size1 =+ i;
			newblk(&amp;dbc, db, &amp;ibc, ib);
		}
		close(f);
		break;

	case IFBLK:
	case IFCHR:
		/*
		 * special file
		 * content is maj/min types
		 */

		in.i_addr[0] = getnum()&lt;&lt;8;
		in.i_addr[0] =| getnum();
		break;

	case IFDIR:
		/*
		 * directory
		 * put in extra links
		 * call recursively until
		 * name of &quot;$&quot; found
		 */

		par-&gt;i_nlink++;
		entry(par-&gt;i_number, &quot;..&quot;, &amp;dbc, db, &amp;ibc, ib);
		in.i_nlink++;
		entry(in.i_number, &quot;.&quot;, &amp;dbc, db, &amp;ibc, ib);
		in.i_size1 = 32;
		for(;;) {
			getstr();
			if(string[0]==&#39;$&#39; &amp;&amp; string[1]==&#39;\0&#39;)
				break;
			entry(ino+1, string, &amp;dbc, db, &amp;ibc, ib);
			in.i_size1 =+ 16;
			cfile(&amp;in);
		}
		break;
	}
	if(dbc != 0)
		newblk(&amp;dbc, db, &amp;ibc, ib);
	if(ibc &gt; 8) {
		in.i_mode =| ILARG;
		dbc = alloc();
		wtfs(dbc, ib);
		in.i_addr[0] = dbc;
	} else
	for(i=0; i&lt;ibc; i++)
		in.i_addr[i] = ib[i];
	in.i_time[0] = in.i_time[2] = utime[0];
	in.i_time[1] = in.i_time[3] = utime[1];
	i = in.i_number + 31;
	dbc = ldiv(0, i, 16);
	p1 = &amp;buf[lrem(0, i, 16)*16];
	p2 = &amp;in.i_mode;
	rdfs(dbc, buf);
	for(i=0; i&lt;16; i++)
		*p1++ = *p2++;
	wtfs(dbc, buf);
}

gmode(c, s, m0, m1, m2, m3)
char c, *s;
{
	int i;

	for(i=0; s[i]!=&#39;\0&#39;; i++)
		if(c == s[i])
			return((&amp;m0)[i]);
	if(c == &#39;-&#39;)
		return(0);
	printf(&quot;%c/%s: bad mode\n&quot;, c, string);
	exit();
}

getnum()
{
	int n, i;

	getstr();
	n = 0;
	i = 0;
	for(i=0; string[i]!=&#39;\0&#39;; i++) {
		if(string[i]&lt;&#39;0&#39; || string[i]&gt;&#39;9&#39;) {
			printf(&quot;%s: bad number\n&quot;, string);
			exit();
		}
		n = n*10 + string[i] - &#39;0&#39;;
	}
	return(n);
}

getstr()
{
	int i, c;

loop:
	switch(c=getch()) {

	case &#39; &#39;:
	case &#39;\t&#39;:
	case &#39;\n&#39;:
		goto loop;

	case &#39;\0&#39;:
		printf(&quot;EOF\n&quot;);
		exit();

	case &#39;:&#39;:
		while(getch() != &#39;\n&#39;);
		goto loop;

	}
	i = 0;

	do {
		string[i++] = c;
		c = getch();
	} while(c!=&#39; &#39;&amp;&amp;c!=&#39;\t&#39;&amp;&amp;c!=&#39;\n&#39;&amp;&amp;c!=&#39;\0&#39;);
	string[i] = &#39;\0&#39;;
}

rdfs(bno, bf)
{
	int n;

	seek(fsi, bno, 3);
	n = read(fsi, bf, 512);
	if(n != 512) {
		printf(&quot;read error: %l\n&quot;, bno);
		exit();
	}
}

wtfs(bno, bf)
{
	int n;

	seek(fso, bno, 3);
	n = write(fso, bf, 512);
	if(n != 512) {
		printf(&quot;write error: %l\n&quot;, bno);
		exit();
	}
}

alloc()
{
	int bno, i;

	filsys.s_nfree--;
	bno = filsys.s_free[filsys.s_nfree];
	filsys.s_free[filsys.s_nfree] = 0;
	if(bno == 0) {
		printf(&quot;out of free space\n&quot;);
		exit();
	}
	if(filsys.s_nfree &lt;= 0) {
		rdfs(bno, buf);
		filsys.s_nfree = buf[0];
		for(i=0; i&lt;100; i++)
			filsys.s_free[i] = buf[i+1];
	}
	return(bno);
}

free(bno)
{
	int i;

	if(filsys.s_nfree &gt;= 100) {
		buf[0] = filsys.s_nfree;
		for(i=0; i&lt;100; i++)
			buf[i+1] = filsys.s_free[i];
		wtfs(bno, buf);
		filsys.s_nfree = 0;
	}
	filsys.s_free[filsys.s_nfree] = bno;
	filsys.s_nfree++;
}

entry(ino, str, adbc, db, aibc, ib)
char *str;
int *adbc, *db, *aibc, *ib;
{
	char *s;
	int i;

	db[*adbc] = ino;
	(*adbc)++;
	s = &amp;db[*adbc];
	for(i=0; i&lt;14; i++) {
		*s++ = *str;
		if(*str != &#39;\0&#39;)
			str++;
	}
	*adbc =+ 7;
	if(*adbc &gt;= 256)
		newblk(adbc, db, aibc, ib);
}

newblk(adbc, db, aibc, ib)
int *adbc, *db, *aibc, *ib;
{
	int bno, i;

	bno = alloc();
	wtfs(bno, db);
	for(i=0; i&lt;256; i++)
		db[i] = 0;
	*adbc = 0;
	ib[*aibc] = bno;
	(*aibc)++;
	if(*aibc &gt;= 256) {
		printf(&quot;indirect block full\n&quot;);
		exit();
	}
}

getch()
{

	if(charp)
		return(*charp++);
	return(getchar());
}

bflist()
{
	char flg[100], adr[100];
	register i, j;
	char *low, *high;

	if(f_n &gt; 100)
		f_n = 100;
	for(i=0; i&lt;f_n; i++)
		flg[i] = 0;
	i = 0;
	for(j=0; j&lt;f_n; j++) {
		while(flg[i])
			i = (i+1)%f_n;
		adr[j] = i;
		flg[i]++;
		i = (i+f_m)%f_n;
	}

	high = filsys.s_fsize-1;
	low = filsys.s_isize+2;
	free(0);
	for(i=high; lrem(0,i+1,f_n); i--) {
		if(i &lt; low)
			break;
		free(i);
	}
	for(; i &gt;= low+f_n; i =- f_n)
		for(j=0; j&lt;f_n; j++)
			free(i-adr[j]);
	for(;i &gt;= low; i--)
		free(i);
}
