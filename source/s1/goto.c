int	offset	0;

main(argc, argv)
char *argv[];
{
	extern fin;
	char line[64];

	if (argc&lt;2 || ttyn(0)!=&#39;x&#39;) {
		write(1, &quot;goto error\n&quot;, 11);
		seek(0, 0, 2);
		return;
	}
	seek(0, 0, 0);
	fin = dup(0);

loop:
	if (getlin(line)) {
		write(1, &quot;label not found\n&quot;, 16);
		return;
		}
	if (compar(line, argv[1])) goto loop;
	seek(0, offset, 0);
}

getlin(s)
char s[];
{
	int ch, i;

	i = 0;
l:
	if ((ch=getc())==&#39;\0&#39;) return(1);
	if (ch!=&#39;:&#39;) {
		while(ch!=&#39;\n&#39; &amp;&amp; ch!=&#39;\0&#39;)
			ch = getc();
		goto l;
		}
	while ((ch=getc())==&#39; &#39;);
	while (ch!=&#39; &#39; &amp;&amp; ch!=&#39;\n&#39; &amp;&amp; ch!=&#39;\0&#39;) {
		s[i++] = ch;
		ch = getc();
		}
	while(ch != &#39;\n&#39;)
		ch = getc();
	s[i] = &#39;\0&#39;;
	return(0);
}

compar(s1, s2)
char s1[], s2[];
{
	int c, i;

	i = 0;
l:
	if(s1[i] != s2[i]) return(1);
	if (s1[i++] == &#39;\0&#39;) return(0);
	goto l;
}

getc()
{

	offset++;
	return(getchar());
}
