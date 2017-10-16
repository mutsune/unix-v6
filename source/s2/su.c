/* su -- become super-user */

char	password[100];
char	pwbuf[100];
int	ttybuf[3];
main()
{
	register char *p, *q;
	extern fin;

	if(getpw(0, pwbuf))
		goto badpw;
	(&amp;fin)[1] = 0;
	p = pwbuf;
	while(*p != &#39;:&#39;)
		if(*p++ == &#39;\0&#39;)
			goto badpw;
	if(*++p == &#39;:&#39;)
		goto ok;
	gtty(0, ttybuf);
	ttybuf[2] =&amp; ~010;
	stty(0, ttybuf);
	printf(&quot;password: &quot;);
	q = password;
	while((*q = getchar()) != &#39;\n&#39;)
		if(*q++ == &#39;\0&#39;)
			return;
	*q = &#39;\0&#39;;
	ttybuf[2] =| 010;
	stty(0, ttybuf);
	printf(&quot;\n&quot;);
	q = crypt(password);
	while(*q++ == *p++);
	if(*--q == &#39;\0&#39; &amp;&amp; *--p == &#39;:&#39;)
		goto ok;
	goto error;

badpw:
	printf(&quot;bad password file\n&quot;);
ok:
	setuid(0);
	execl(&quot;/bin/sh&quot;, &quot;-&quot;, 0);
	printf(&quot;cannot execute shell\n&quot;);
error:
	printf(&quot;sorry\n&quot;);
}
