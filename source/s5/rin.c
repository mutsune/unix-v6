rin()
{
	double d;
	register n, c, f;

	d = 0.;
	f = 0;
	n = 0;
loop:
	c = getchar();
	if(c == &#39;\0&#39;)
		exit();
	if(c == &#39;-&#39;) {
		f++;
		goto loop;
	}
	if(c == &#39;.&#39;) {
		n++;
		goto loop;
	}
	if(c&gt;=&#39;0&#39; &amp;&amp; c&lt;=&#39;9&#39;) {
		if(n)
			n++;
		d = d*10.+c-&#39;0&#39;;
		goto loop;
	}
	if(f)
		d = -d;
	for(f=1; f&lt;n; f++)
		d =/ 10.;
	n = d;
	return(n);
}
