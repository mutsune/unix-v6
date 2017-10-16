#define	ANY	-1
#define	LIST	-2
#define	RANGE	-3
#define	EOF	-4
char	*crontab	&quot;/usr/lib/crontab&quot;;
char	*crontmp	&quot;/tmp/crontmp&quot;;
char	*aend;
char	*itime[2];
int	*loct;
int	*localtime();
int	reset();
int	flag;

main()
{
	register i, t;
	register char *cp;
	extern char end[];

	setuid(1);
	itime[0] = fork();
	if(itime[0])
		exit();
	setexit();
	signal(1, reset);
	time(itime);
	while(*localtime(itime))
		if(itime[1]-- == 0)
			itime[0]--;

loop:
	init();
	for(i=60; i; i--) {
		loct = localtime(itime);
		for(cp = end; *cp != EOF;) {
			flag = 0;
			cp = cmp(cp, loct[1]); /* minute */
			cp = cmp(cp, loct[2]); /* hour */
			cp = cmp(cp, loct[3]); /* day */
			cp = cmp(cp, loct[4]); /* month */
			cp = cmp(cp, loct[6]); /* day of week */
			if(flag == 0) {
				slp();
				ex(cp);
			}
			while(*cp++ != 0)
				;
		}
		t = itime[1] + 60;
		if(t &lt; itime[1])
			itime[0]++;
		itime[1] = t;
	}
	slp();
	goto loop;
}

cmp(p, v)
char *p;
{
	register char *cp;

	cp = p;
	switch(*cp++) {

	case ANY:
		return(cp);

	case LIST:
		while(*cp != LIST)
			if(*cp++ == v) {
				while(*cp++ != LIST)
					;
				return(cp);
			}
		flag++;
		return(cp+1);

	case RANGE:
		if(*cp &gt; v || cp[1] &lt; v)
			flag++;
		return(cp+2);
	}
	if(cp[-1] != v)
		flag++;
	return(cp);
}

slp()
{
	register i;
	int	t[2];

	time(t);
	i = itime[1] - t[1];
	if(i &gt; 0)
		sleep(i);
}

ex(s)
char *s;
{
	register i;

	if(fork()) {
		wait();
		return;
	}
	for(i=0; s[i]; i++);
	close(0);
	creat(crontmp, 0600);
	write(0, s, i);
	close(0);
	open(crontmp, 0);
	unlink(crontmp);
	if(fork())
		exit();
	execl(&quot;/bin/sh&quot;, &quot;sh&quot;, &quot;-t&quot;, 0);
	exit();
}

init()
{
	int ib[259], t[10];
	register i, c;
	register char *cp;
	char *ocp;
	int n;
	extern char end[];

	if(fopen(crontab, ib) &lt; 0) {
		write(1, &quot;cannot open table\n&quot;, 18);
		exit();
	}
	cp = end;
	if(aend == 0)
		aend = cp;

loop:
	ocp = cp;
	if(cp+100 &gt; aend) {
		aend =+ 512;
		brk(aend);
	}
	for(i=0;; i++) {
		do
			c = getc(ib);
		while(c == &#39; &#39; || c == &#39;\t&#39;);
		if(c &lt;= 0 || c == &#39;\n&#39;)
			goto ignore;
		if(i == 5)
			break;
		if(c == &#39;*&#39;) {
			*cp++ = ANY;
			continue;
		}
		n = 0;
		while(c &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;) {
			n = n*10 + c-&#39;0&#39;;
			c = getc(ib);
		}
		if(n &lt; 0 || n &gt; 100)
			goto ignore;
		if(c == &#39;,&#39;)
			goto list;
		if(c == &#39;-&#39;)
			goto range;
		if(c != &#39;\t&#39; &amp;&amp; c != &#39; &#39;)
			goto ignore;
		*cp++ = n;
		continue;

	list:
		*cp++ = LIST;
		*cp++ = n;
	list1:
		n = 0;
		c = getc(ib);
		while(c &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;) {
			n = n*10 + c-&#39;0&#39;;
			c = getc(ib);
		}
		if(n &lt; 0 || n &gt; 100)
			goto ignore;
		*cp++ = n;
		if(c == &#39;,&#39;)
			goto list1;
		if(c != &#39;\t&#39; &amp;&amp; c != &#39; &#39;)
			goto ignore;
		*cp++ = LIST;
		continue;

	range:
		*cp++ = RANGE;
		*cp++ = n;
		n = 0;
		c = getc(ib);
		while(c &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;) {
			n = n*10 + c-&#39;0&#39;;
			c = getc(ib);
		}
		if(n &lt; 0 || n &gt; 100)
			goto ignore;
		if(c != &#39;\t&#39; &amp;&amp; c != &#39; &#39;)
			goto ignore;
		*cp++ = n;
	}
	while(c != &#39;\n&#39;) {
		if(c &lt;= 0)
			goto ignore;
		if(c == &#39;%&#39;)
			c = &#39;\n&#39;;
		*cp++ = c;
		c = getc(ib);
	}
	*cp++ = &#39;\n&#39;;
	*cp++ = 0;
	goto loop;

ignore:
	cp = ocp;
	while(c != &#39;\n&#39;) {
		if(c &lt;= 0) {
			close(ib[0]);
			*cp++ = EOF;
			*cp++ = EOF;
			aend = cp;
			brk(aend);
			return;
		}
		c = getc(ib);
	}
	goto loop;
}
