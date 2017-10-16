atoi(ap)
char *ap;
{
	register n, c;
	register char *p;
	int f;

	p = ap;
	n = 0;
	f = 0;
loop:
	while(*p == &#39; &#39; || *p == &#39;	&#39;)
		p++;
	if(*p == &#39;-&#39;) {
		f++;
		p++;
		goto loop;
	}
	while(*p &gt;= &#39;0&#39; &amp;&amp; *p &lt;= &#39;9&#39;)
		n = n*10 + *p++ - &#39;0&#39;;
	if(f)
		n = -n;
	return(n);
}
