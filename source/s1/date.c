int	timbuf[2];
char	*cbp;

char *tzname[2];
int	dmsize[];
char	cbuf[];
char	*cbp;

struct {
	char	name[8];
	char	tty;
	char	fill1;
	int	wtime[2];
	int	fill2;
} wtmp[2];

main(argc, argv)
int argc, **argv;
{
	register char *tzn;
	extern int timezone, *localtime();
	int wf, tfailed;

	tfailed = 0;
	if(argc &gt; 1) {
		cbp = argv[1];
		if(gtime()) {
			write(1, &quot;bad conversion\n&quot;, 15);
			exit();
		}
		if (*cbp != &#39;s&#39;) {
	/* convert to Greenwich time, on assumption of Standard time. */
			dpadd(timbuf, timezone);
	/* Now fix up to local daylight time. */
			if (localtime(timbuf)[8])
				dpadd(timbuf, -1*60*60);
		}
		time(wtmp[0].wtime);
		wtmp[0].tty =  &#39;|&#39;;
		if(stime(timbuf) &lt; 0) {
			tfailed++;
			write(1, &quot;no permission\n&quot;, 14);
		} else if ((wf = open(&quot;/usr/adm/wtmp&quot;, 1)) &gt;= 0) {
			time(wtmp[1].wtime);
			wtmp[1].tty = &#39;}&#39;;
			seek(wf, 0, 2);
			write(wf, wtmp, 32);
		}
	}
	if (tfailed==0)
		time(timbuf);
	cbp = cbuf;
	ctime(timbuf);
	write(1, cbuf, 20);
	tzn = tzname[localtime(timbuf)[8]];
	if (tzn)
		write(1, tzn, 3);
	write(1, cbuf+19, 6);
}

gtime()
{
	register int i;
	register int y, t;
	int d, h, m;
	extern int *localtime();
	int nt[2];

	t = gpair();
	if(t&lt;1 || t&gt;12)
		goto bad;
	d = gpair();
	if(d&lt;1 || d&gt;31)
		goto bad;
	h = gpair();
	if(h == 24) {
		h = 0;
		d++;
	}
	m = gpair();
	if(m&lt;0 || m&gt;59)
		goto bad;
	y = gpair();
	if (y&lt;0) {
		time(nt);
		y = localtime(nt)[5];
	}
	if (*cbp == &#39;p&#39;)
		h =+ 12;
	if (h&lt;0 || h&gt;23)
		goto bad;
	timbuf[0] = 0;
	timbuf[1] = 0;
	y =+ 1900;
	for(i=1970; i&lt;y; i++)
		gdadd(dysize(i));
	/* Leap year */
	if (dysize(y)==366 &amp;&amp; t &gt;= 3)
		gdadd(1);
	while(--t)
		gdadd(dmsize[t-1]);
	gdadd(d-1);
	gmdadd(24, h);
	gmdadd(60, m);
	gmdadd(60, 0);
	return(0);

bad:
	return(1);
}

gdadd(n)
{
	register char *t;

	t = timbuf[1]+n;
	if(t &lt; timbuf[1])
		timbuf[0]++;
	timbuf[1] = t;
}

gmdadd(m, n)
{
	register int t1;

	timbuf[0] =* m;
	t1 = timbuf[1];
	while(--m)
		gdadd(t1);
	gdadd(n);
}

gpair()
{
	register int c, d;
	register char *cp;

	cp = cbp;
	if(*cp == 0)
		return(-1);
	c = (*cp++ - &#39;0&#39;) * 10;
	if (c&lt;0 || c&gt;100)
		return(-1);
	if(*cp == 0)
		return(-1);
	if ((d = *cp++ - &#39;0&#39;) &lt; 0 || d &gt; 9)
		return(-1);
	cbp = cp;
	return (c+d);
}
