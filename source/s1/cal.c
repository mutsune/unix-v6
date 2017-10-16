char	dayw[]
{
	&quot; S  M Tu  W Th  F  S&quot;
};
char	*smon[]
{
	&quot;Jan&quot;, &quot;Feb&quot;, &quot;Mar&quot;, &quot;Apr&quot;,
	&quot;May&quot;, &quot;Jun&quot;, &quot;Jul&quot;, &quot;Aug&quot;,
	&quot;Sep&quot;, &quot;Oct&quot;, &quot;Nov&quot;, &quot;Dec&quot;,
};
char	string[432];
main(argc, argv)
char *argv[];
{
	register y, i, j;
	int m;

	if(argc &lt; 2) {
		printf(&quot;usage: cal [month] year\n&quot;);
		exit();
	}
	if(argc == 2)
		goto xlong;

/*
 *	print out just month
 */

	m = number(argv[1]);
	if(m&lt;1 || m&gt;12)
		goto badarg;
	y = number(argv[2]);
	if(y&lt;1 || y&gt;9999)
		goto badarg;
	printf(&quot;      %s %l\n&quot;, smon[m-1], y);
	printf(&quot;%s\n&quot;, dayw);
	cal(m, y, string, 24);
	for(i=0; i&lt;6*24; i=+24)
		pstr(string+i, 24);
	exit();

/*
 *	print out complete year
 */

xlong:
	y = number(argv[1]);
	if(y&lt;1 || y&gt;9999)
		goto badarg;
	printf(&quot;\n\n\n&quot;);
	printf(&quot;				%l\n&quot;, y);
	printf(&quot;\n&quot;);
	for(i=0; i&lt;12; i=+3) {
		for(j=0; j&lt;6*72; j++)
			string[j] = &#39;\0&#39;;
		printf(&quot;	 %s&quot;, smon[i]);
		printf(&quot;			%s&quot;, smon[i+1]);
		printf(&quot;		       %s\n&quot;, smon[i+2]);
		printf(&quot;%s   %s   %s\n&quot;, dayw, dayw, dayw);
		cal(i+1, y, string, 72);
		cal(i+2, y, string+23, 72);
		cal(i+3, y, string+46, 72);
		for(j=0; j&lt;6*72; j=+72)
			pstr(string+j, 72);
	}
	printf(&quot;\n\n\n&quot;);
	exit();

badarg:
	printf(&quot;Bad argument\n&quot;);
}

number(str)
char *str;
{
	register n, c;
	register char *s;

	n = 0;
	s = str;
	while(c = *s++) {
		if(c&lt;&#39;0&#39; || c&gt;&#39;9&#39;)
			return(0);
		n = n*10 + c-&#39;0&#39;;
	}
	return(n);
}

pstr(str, n)
char *str;
{
	register i;
	register char *s;

	s = str;
	i = n;
	while(i--)
		if(*s++ == &#39;\0&#39;)
			s[-1] = &#39; &#39;;
	i = n+1;
	while(i--)
		if(*--s != &#39; &#39;)
			break;
	s[1] = &#39;\0&#39;;
	printf(&quot;%s\n&quot;, str);
}

char	mon[]
{
	0,
	31, 29, 31, 30,
	31, 30, 31, 31,
	30, 31, 30, 31,
};

cal(m, y, p, w)
char *p;
{
	register d, i;
	register char *s;

	s = p;
	d = jan1(y);
	mon[2] = 29;
	mon[9] = 30;

	switch((jan1(y+1)+7-d)%7) {

	/*
	 *	non-leap year
	 */
	case 1:
		mon[2] = 28;
		break;

	/*
	 *	1752
	 */
	default:
		mon[9] = 19;
		break;

	/*
	 *	leap year
	 */
	case 2:
		;
	}
	for(i=1; i&lt;m; i++)
		d =+ mon[i];
	d =% 7;
	s =+ 3*d;
	for(i=1; i&lt;=mon[m]; i++) {
		if(i==3 &amp; mon[m]==19) {
			i =+ 11;
			mon[m] =+ 11;
		}
		if(i &gt; 9)
			*s = i/10+&#39;0&#39;;
		s++;
		*s++ = i%10+&#39;0&#39;;
		s++;
		if(++d == 7) {
			d = 0;
			s = p+w;
			p = s;
		}
	}
}

/*
 *	return day of the week
 *	of jan 1 of given year
 */

jan1(yr)
{
	register y, d;

/*
 *	normal gregorian calendar
 *	one extra day per four years
 */

	y = yr;
	d = 4+y+(y+3)/4;

/*
 *	julian calendar
 *	regular gregorian
 *	less three days per 400
 */

	if(y &gt; 1800) {
		d =- (y-1701)/100;
		d =+ (y-1601)/400;
	}

/*
 *	great calendar changeover instant
 */

	if(y &gt; 1752)
		d =+ 3;

	return(d%7);
}
