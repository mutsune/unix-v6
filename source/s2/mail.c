#
/* mail command usage
	mail
		prints your mail
	mail people
		sends standard input to people
 */

#define	SIGINT	2

struct	utmp {
	char	name[8];
	char	tty;
	char	pad1;
	int	ltime[2];
	int	pad2;
};

struct	passwd {
	char	*pw_name;
	char	*pw_passwd;
	int	pw_uid;
	int	pw_gid;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

char	lettmp[] &quot;/tmp/maxxxxx&quot;;
char	preptmp[] &quot;/tmp/mbxxxxx&quot;;
int	pwfil;

main(argc, argv)
char **argv;
{
	register me;
	extern fout;
	register struct passwd *p;
	register char *cp;
	static struct utmp ubuf;
	int uf;

	maketemp();
	if (argc==1 || argc==2 &amp;&amp; argv[1][0]==&#39;-&#39;) {
		printmail(argc, argv);
		delexit();
	}
	signal(SIGINT, delexit);
	fout = creat(lettmp, 0600);
	if (((me=ttyn(1))!=&#39;x&#39; || (me=ttyn(2))!=&#39;x&#39;)
	 &amp;&amp; (uf = open(&quot;/etc/utmp&quot;, 0)) &gt; 0) {
		while (read(uf, &amp;ubuf, sizeof ubuf) == sizeof ubuf)
			if (ubuf.tty == me) {
				ubuf.name[8] = &#39; &#39;;
				close(uf);
				for (cp=ubuf.name; *cp++!=&#39; &#39;;);
				*--cp = 0;
				bulkmail(argc, argv, ubuf.name);
			}
	}
	me = getuid() &amp; 0377;
	setpw();
	for (;;)
		if ((p = getpwent()) &amp;&amp; p-&gt;pw_uid == me)
			bulkmail(argc, argv, p-&gt;pw_name);
	fout = 1;
	printf(&quot;Who are you?\n&quot;);
	delexit();
}

printmail(argc, argv)
char **argv;
{
	extern fin, fout;
	register n, c, f;

	if (fopen(&quot;.mail&quot;, &amp;fin)&gt;=0 &amp;&amp; (c = getchar())) {
		do {
			putchar(c);
		} while (c = getchar());
		close(fin);
		c = &#39;y&#39;;
		if (argc&lt;2) {
			if (ttyn(0)!=&#39;x&#39;) {
				printf(&quot;Save?&quot;);
				fin = 0;
				c = getchar();
			}
		} else
			c = argv[1][1];
		if (c==&#39;y&#39;) {
			prepend(&quot;.mail&quot;, &quot;mbox&quot;);
			printf(&quot;Saved mail in &#39;mbox&#39;\n&quot;);
		}
		close(creat(&quot;.mail&quot;));
	} else
		printf(&quot;No mail.\n&quot;);
}

bulkmail(argc, argv, from)
char **argv, *from;
{
	extern fin, fout;
	int tbuf[2];
	register c;

	fin = 0;
	(&amp;fin)[1] = 0;
	time(tbuf);
	printf(&quot;From %s %s&quot;, from, ctime(tbuf));
	while (c = getchar())
		putchar(c);
	putchar(&#39;\n&#39;);
	flush();
	close(fout);
	while (--argc &gt; 0)
		sendto(*++argv);
	delexit();
}

sendto(person)
char *person;
{
	static saved;
	extern fout;
	extern fin;
	register struct passwd *p;

	setpw();
	while (p = getpwent()) {
		if (equal(p-&gt;pw_name, person)) {
			if (prepend(lettmp, cat(p-&gt;pw_dir, &quot;/.mail&quot;))==0)
				break;
			return;
		}
	}
	fout = 1;
	flush();
	printf(&quot;Can&#39;t send to %s.\n&quot;, person);
	if (ttyn(0)!=&#39;x&#39; &amp;&amp; saved==0) {
		unlink(&quot;dead.letter&quot;);
		saved++;
		printf(&quot;Letter saved in &#39;dead.letter&#39;\n&quot;);
		prepend(lettmp, &quot;dead.letter&quot;);
	}
}

prepend(from, to)
char *from, *to;
{
	extern int fin, fout;

	fcreat(preptmp, &amp;fout);
	fopen(from, &amp;fin);
	while (putchar(getchar()));
	close(fin);
	fopen(to, &amp;fin);
	while (putchar(getchar()));
	close(fin);
	flush();
	close(fout);
	if (fcreat(to, &amp;fout) &lt; 0) {
		fout = 1;
		return(0);
	}
	fopen(preptmp, &amp;fin);
	while(putchar(getchar()));
	flush();
	close(fout);
	close(fin);
	fout = 1;
	return(1);
}

setpw()
{
	extern fin;

	if (pwfil == 0) {
		fopen(&quot;/etc/passwd&quot;, &amp;fin);
		pwfil = fin;
	} else
		fin = pwfil;
	(&amp;fin)[1] = 0;
	seek(fin, 0, 0);
}

getpwent()
{
	register char *p;
	register c;
	static struct passwd passwd;
	static char line[100];
	extern fin;

	p = line;
	while((c=getchar()) != &#39;\n&#39;) {
		if(c &lt;= 0)
			return(0);
		if(p &lt; line+98)
			*p++ = c;
	}
	*p = 0;
	p = line;
	passwd.pw_name = p;
	p = pwskip(p);
	passwd.pw_passwd = p;
	p = pwskip(p);
	passwd.pw_uid = atoi(p);
	p = pwskip(p);
	passwd.pw_gid = atoi(p);
	p = pwskip(p);
	passwd.pw_gecos = p;
	p = pwskip(p);
	passwd.pw_dir = p;
	p = pwskip(p);
	passwd.pw_shell = p;
	return(&amp;passwd);
}

pwskip(ap)
char *ap;
{
	register char *p;

	p = ap;
	while(*p != &#39;:&#39;) {
		if(*p == 0)
			return(p);
		p++;
	}
	*p++ = 0;
	return(p);
}

delexit()
{
	unlink(lettmp);
	unlink(preptmp);
	exit(0);
}

maketemp()
{
	int i, pid, d;

	pid = getpid();
	for (i=11; i&gt;=7; --i) {
		d = (pid&amp;07) + &#39;0&#39;;
		lettmp[i] = d;
		preptmp[i] = d;
		pid =&gt;&gt; 3;
	}
}

equal(as1, as2)
{
	register char *s1, *s2;

	s1 = as1;
	s2 = as2;
	while (*s1++ == *s2)
		if (*s2++ == 0)
			return(1);
	return(0);
}

cat(ap1, ap2)
char *ap1, *ap2;
{
	register char *p1, *p2;
	static char fn[32];

	p1 = ap1;
	p2 = fn;
	while (*p2++ = *p1++);
	p2--;
	p1 = ap2;
	while (*p2++ = *p1++);
	return(fn);
}
