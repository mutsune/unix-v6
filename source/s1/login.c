#
/*
 * login [ name ]
 */

struct {
	char	name[8];
	char	tty;
	char	ifill;
	int	time[2];
	int	ufill;
} utmp;

struct {
	int	speeds;
	char	erase, kill;
	int	tflags;
} ttyb;

struct {
	int	junk[5];
	int	size;
	int	more[12];
} statb;

char	*ttyx;

#define	ECHO	010

main(argc, argv)
char **argv;
{
	char pbuf[128];
	register char *namep, *np;
	char pwbuf[9];
	int t, sflags, f, c, uid, gid;

	signal(3, 1);
	signal(2, 1);
	nice(0);
	ttyx = &quot;/dev/ttyx&quot;;
	if ((utmp.tty=ttyn(0)) == &#39;x&#39;) {
		write(1, &quot;Sorry.\n&quot;, 7);
		exit();
	}
	ttyx[8] = utmp.tty;
	gtty(0, &amp;ttyb);
	ttyb.erase = &#39;#&#39;;
	ttyb.kill = &#39;@&#39;;
	stty(0, &amp;ttyb);
    loop:
	namep = utmp.name;
	if (argc&gt;1) {
		np = argv[1];
		while (namep&lt;utmp.name+8 &amp;&amp; *np)
			*namep++ = *np++;
		argc = 0;
	} else {
		write(1, &quot;Name: &quot;, 7);
		while ((c = getchar()) != &#39;\n&#39;) {
			if (c &lt;= 0)
				exit();
			if (namep &lt; utmp.name+8)
				*namep++ = c;
		}
	}
	while (namep &lt; utmp.name+8)
		*namep++ = &#39; &#39;;
	if (getpwentry(utmp.name, pbuf))
		goto bad;
	np = colon(pbuf);
	if (*np!=&#39;:&#39;) {
		sflags = ttyb.tflags;
		ttyb.tflags =&amp; ~ ECHO;
		stty(0, &amp;ttyb);
		write(1, &quot;Password: &quot;, 10);
		namep = pwbuf;
		while ((c=getchar()) != &#39;\n&#39;) {
			if (c &lt;= 0)
				exit();
			if (namep&lt;pwbuf+8)
				*namep++ = c;
		}
		*namep++ = &#39;\0&#39;;
		ttyb.tflags = sflags;
		stty(0, &amp;ttyb);
		write(1, &quot;\n&quot;, 1);
		namep = crypt(pwbuf);
		while (*namep++ == *np++);
		if (*--namep!=&#39;\0&#39; || *--np!=&#39;:&#39;)
			goto bad;
	}
	np = colon(np);
	uid = 0;
	while (*np != &#39;:&#39;)
		uid = uid*10 + *np++ - &#39;0&#39;;
	np++;
	gid = 0;
	while (*np != &#39;:&#39;)
		gid = gid*10 + *np++ - &#39;0&#39;;
	np++;
	np = colon(np);
	namep = np;
	np = colon(np);
	if (chdir(namep)&lt;0) {
		write(1, &quot;No directory\n&quot;, 13);
		goto loop;
	}
	time(utmp.time);
	if ((f = open(&quot;/etc/utmp&quot;, 1)) &gt;= 0) {
		t = utmp.tty;
		if (t&gt;=&#39;a&#39;)
			t =- &#39;a&#39; - (10+&#39;0&#39;);
		seek(f, (t-&#39;0&#39;)*16, 0);
		write(f, &amp;utmp, 16);
		close(f);
	}
	if ((f = open(&quot;/usr/adm/wtmp&quot;, 1)) &gt;= 0) {
		seek(f, 0, 2);
		write(f, &amp;utmp, 16);
		close(f);
	}
	if ((f = open(&quot;/etc/motd&quot;, 0)) &gt;= 0) {
		while(read(f, &amp;t, 1) &gt; 0)
			write(1, &amp;t, 1);
		close(f);
	}
	if(stat(&quot;.mail&quot;, &amp;statb) &gt;= 0 &amp;&amp; statb.size)
		write(1, &quot;You have mail.\n&quot;, 15);
	chown(ttyx, uid);
	setgid(gid);
	setuid(uid);
	if (*np == &#39;\0&#39;)
		np = &quot;/bin/sh&quot;;
	execl(np, &quot;-&quot;, 0);
	write(1, &quot;No shell.\n&quot;, 9);
	exit();
bad:
	write(1, &quot;Login incorrect.\n&quot;, 17);
	goto loop;
}

getpwentry(name, buf)
char *name, *buf;
{
	extern fin;
	int fi, r, c;
	register char *gnp, *rnp;

	fi = fin;
	r = 1;
	if((fin = open(&quot;/etc/passwd&quot;, 0)) &lt; 0)
		goto ret;
loop:
	gnp = name;
	rnp = buf;
	while((c=getchar()) != &#39;\n&#39;) {
		if(c &lt;= 0)
			goto ret;
		*rnp++ = c;
	}
	*rnp++ = &#39;\0&#39;;
	rnp = buf;
	while (*gnp++ == *rnp++);
	if ((*--gnp!=&#39; &#39; &amp;&amp; gnp&lt;name+8) || *--rnp!=&#39;:&#39;)
		goto loop;
	r = 0;
ret:
	close(fin);
	fin = 0;
	(&amp;fin)[1] = 0;
	(&amp;fin)[2] = 0;
	return(r);
}

colon(p)
char *p;
{
	register char *rp;

	rp = p;
	while (*rp != &#39;:&#39;) {
		if (*rp++ == &#39;\0&#39;) {
			write(1, &quot;Bad /etc/passwd\n&quot;, 16);
			exit();
		}
	}
	*rp++ = &#39;\0&#39;;
	return(rp);
}
