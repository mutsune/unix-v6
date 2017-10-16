getpw(uid, buf)
int uid;
char buf[];
{
	auto pbuf[259];
	static pwf;
	register n, c;
	register char *bp;

	if(pwf == 0)
		pwf = open(&quot;/etc/passwd&quot;, 0);
	if(pwf &lt; 0)
		return(1);
	seek(pwf, 0, 0);
	pbuf[0] = pwf;
	pbuf[1] = 0;
	pbuf[2] = 0;
	uid =&amp; 0377;

	for (;;) {
		bp = buf;
		while((c=getc(pbuf)) != &#39;\n&#39;) {
			if(c &lt;= 0)
				return(1);
			*bp++ = c;
		}
		*bp++ = &#39;\0&#39;;
		bp = buf;
		n = 3;
		while(--n)
		while((c = *bp++) != &#39;:&#39;)
			if(c == &#39;\n&#39;)
				return(1);
		while((c = *bp++) != &#39;:&#39;) {
			if(c&lt;&#39;0&#39; || c&gt;&#39;9&#39;)
				continue;
			n = n*10+c-&#39;0&#39;;
		}
		if(n == uid)
			return(0);
	}
	return(1);
}
