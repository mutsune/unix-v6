#
/*
 * acct [ -w wtmp ] [ -d ] [ -p people ]
 */

#define	TSIZE	30
#define	USIZE	200

struct {
	char	name[8];
	char	tty;
	char	fill1;
	float	time;
	int	fill2;
} ibuf;

struct ubuf {
	char	name[8];
	float	utime;
} ubuf[USIZE];

struct tbuf {
	struct	ubuf	*userp;
	float	ttime;
} tbuf[TSIZE];

char	*wtmp;
int	pflag, byday;
double	dtime;
double	midnight;
double	lastime;
double	day	1440.;
int	pcount;
char	**pptr;

int	montab[] { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

char	*monasc[] {
	&quot;Jan&quot;,
	&quot;Feb&quot;,
	&quot;Mar&quot;,
	&quot;Apr&quot;,
	&quot;May&quot;,
	&quot;Jun&quot;,
	&quot;Jul&quot;,
	&quot;Aug&quot;,
	&quot;Sep&quot;,
	&quot;Oct&quot;,
	&quot;Nov&quot;,
	&quot;Dec&quot;
};

main(argc, argv) 
char **argv;
{
	int c, fl;
	register i;
	register char *ip;
	extern fin;
	int f;

	wtmp = &quot;/usr/adm/wtmp&quot;;
	while (--argc &gt; 0 &amp;&amp; **++argv == &#39;-&#39;)
	switch(*++*argv) {
	case &#39;d&#39;:
		byday++;
		continue;

	case &#39;w&#39;:
		if (--argc&gt;0)
			wtmp = *++argv;
		continue;

	case &#39;p&#39;:
		pflag++;
		continue;
	}
	pcount = argc;
	pptr = argv;
	if (fopen(wtmp, &amp;fin) &lt; 0) {
		printf(&quot;No %s\n&quot;, wtmp);
		return;
	}
	for(;;) {
		ip = &amp;ibuf;
		for (i=0; i&lt;16; i++) {
			if ((c=getc(&amp;fin)) &lt; 0)
				goto brk;
			*ip++ = c;
		}
		fl = 0;
		for (i=0; i&lt;8; i++) {
			c = ibuf.name[i];
			if (&#39;0&#39;&lt;=c&amp;&amp;c&lt;=&#39;9&#39;||&#39;a&#39;&lt;=c&amp;&amp;c&lt;=&#39;z&#39;||&#39;A&#39;&lt;=c&amp;&amp;c&lt;=&#39;Z&#39;) {
				if (fl)
					goto skip;
				continue;
			}
			if (c==&#39; &#39; || c==&#39;\0&#39;) {
				fl++;
				ibuf.name[i] = &#39;\0&#39;;
			} else
				goto skip;
		}
		loop();
    skip:;
	}
    brk:
	ibuf.name[0] = &#39;\0&#39;;
	ibuf.tty = &#39;~&#39;;
	time(&amp;ibuf.time);
	loop();
	print();
}

loop()
{
	double ltod();
	register i;
	register struct tbuf *tp;
	register struct ubuf *up;

	if (ibuf.fill1||ibuf.fill2)
		return;
	ibuf.time = ltod(&amp;ibuf.time)/60.;
	if(ibuf.tty == &#39;|&#39;) {
		dtime = ibuf.time;
		return;
	}
	if(ibuf.tty == &#39;}&#39;) {
		if(dtime == 0.)
			return;
		for(tp = tbuf; tp &lt; &amp;tbuf[TSIZE]; tp++)
			tp-&gt;ttime =+ ibuf.time-dtime;
		dtime = 0.;
		return;
	}
	if (lastime&gt;ibuf.time || lastime+(1.5*day)&lt;ibuf.time)
		midnight = 0.0;
	if (midnight==0.0)
		newday();
	lastime = ibuf.time;
	if (byday &amp;&amp; ibuf.time &gt; midnight) {
		upall(1);
		print();
		newday();
		for (up=ubuf; up &lt; &amp;ubuf[USIZE]; up++)
			up-&gt;utime = 0.0;
	}
	if (ibuf.tty == &#39;~&#39;) {
		ibuf.name[0] = &#39;\0&#39;;
		upall(0);
		return;
	}
	if ((i = ibuf.tty) &gt;= &#39;a&#39;)
		i =- &#39;a&#39; - &#39;9&#39;;
	i =- &#39;0&#39;;
	if (i&lt;0 || i&gt;=TSIZE)
		i = TSIZE-1;
	tp = &amp;tbuf[i];
	update(tp, 0);
}

print()
{
	int i;
	float ttime, t;

	ttime = 0.0;
	for (i=0; i&lt;USIZE; i++) {
		if(!among(i))
			continue;
		t = ubuf[i].utime;
		if (t&gt;0.0)
			ttime =+ t;
		if (pflag &amp;&amp; ubuf[i].utime &gt; 0.0) {
			printf(&quot;\t%-8.8s%6.2f\n&quot;,
			    ubuf[i].name, ubuf[i].utime/60.);
		}
	}
	if (ttime &gt; 0.0) {
		pdate();
		printf(&quot;\ttotal%9.2f\n&quot;, ttime/60.);
	}
}

upall(f)
{
	register struct tbuf *tp;

	for (tp=tbuf; tp &lt; &amp;tbuf[TSIZE]; tp++)
		update(tp, f);
}

update(tp, f)
struct tbuf *tp;
{
	int i, j;
	struct ubuf *up;
	double t, t1;

	if (f)
		t = midnight;
	else
		t = ibuf.time;
	if (tp-&gt;userp) {
		t1 = t - tp-&gt;ttime;
		if (t1&gt;0.0 &amp;&amp; t1 &lt; 1.5*day)
			tp-&gt;userp-&gt;utime =+ t1;
	}
	tp-&gt;ttime = t;
	if (f)
		return;
	if (ibuf.name[0]==&#39;\0&#39;) {
		tp-&gt;userp = 0;
		return;
	}
	for (up=ubuf; up &lt; &amp;ubuf[USIZE]; up++) {
		if (up-&gt;name[0] == &#39;\0&#39;)
			break;
		for (j=0; j&lt;8 &amp;&amp; up-&gt;name[j]==ibuf.name[j]; j++);
		if (j&gt;=8)
			break;
	}
	for (j=0; j&lt;8; j++)
		up-&gt;name[j] = ibuf.name[j];
	tp-&gt;userp = up;
}

among(i)
{
	register j, k;
	register char *p;

	if (pcount==0)
		return(1);
	for (j=0; j&lt;pcount; j++) {
		p = pptr[j];
		for (k=0; k&lt;8; k++) {
			if (*p == ubuf[i].name[k]) {
				if (*p++ == &#39;\0&#39;)
					return(1);
			} else
				break;
		}
	}
	return(0);
}

newday()
{
	if(midnight == 0.)
		midnight = 240.;
	while (midnight &lt;= ibuf.time)
		midnight =+ day;
}

pdate()
{
	register days, mons, yrs;
	double year, tim;

	if (byday==0)
		return;
	yrs = 2;
	tim = 0.0;
	for(;;) {
		year = 365. * day;
		if(yrs%4 == 0)
			year =+ day;
		if(tim+year &gt; midnight)
			break;
		yrs++;
		tim =+ year;
	}
	days = (midnight-tim-720.)/day;
	montab[1] = 28;
	if(yrs%4 == 0)
		montab[1]++;
	for (mons=0; montab[mons]&lt;=days; mons++)
		days =- montab[mons];
	mons =% 12;
	printf(&quot;%s %2d&quot;, monasc[mons], days+1);
}
