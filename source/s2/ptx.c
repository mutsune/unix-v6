/* permuted title index */

char	*tfil &quot;/tmp/p.tmp&quot;;
char	*sw[] {
	&quot;a&quot;,
	&quot;an&quot;,
	&quot;and&quot;,
	&quot;as&quot;,
	&quot;for&quot;,
	&quot;is&quot;,
	&quot;of&quot;,
	&quot;on&quot;,
	&quot;or&quot;,
	&quot;the&quot;,
	&quot;to&quot;,
	&quot;up&quot;,
	0};
char	line[200];
int	ch;
int	ptflg;
int	llen	72;

main(argc, argv)
int argc;
char *argv[];
{
	extern fin, fout;
	extern onintr();
	int f;

	if(argc&gt;1 &amp;&amp; *argv[1]==&#39;-&#39;) {
		llen = 100;
		ptflg++;
		argc--;
		argv++;
	}
	if(argc&lt;2) {
		printf(&quot;arg count\n&quot;);
		exit();
	}
	fin = open(argv[1]);
	if(fin &lt; 0) {
		printf(&quot;%s: cannot open\n&quot;, argv[1]);
		exit();
	}
	f = creat(tfil, 0600);
	if(f &lt; 0) {
		printf(&quot;cannot create %s\n&quot;, tfil);
		exit();
	}
	fout = f;
	if ((signal(2, 1) &amp; 01) ==0)
		signal(2, onintr);
	pass1();
	flush();
	close(fin);
	fin = 0;
	close(fout);
	fout = 1;
	f = fork();
	if(f &lt; 0) {
		printf(&quot;try again\n&quot;);
		exit();
	}
	if(f == 0) {
		execl(&quot;/bin/sort&quot;, &quot;sort&quot;, &quot;-d&quot;, &quot;-o&quot;, tfil, tfil, 0);
		execl(&quot;/usr/bin/sort&quot;, &quot;sort&quot;, &quot;-d&quot;, &quot;-o&quot;, tfil, tfil, 0);
		printf(&quot;someone moved sort\n&quot;);
		exit();
	}
	while(wait() != f);
	fin = open(tfil, 0);
	if(fin &lt; 0) {
		printf(&quot;cannot reopen %s\n&quot;, tfil);
		exit();
	}
	if (argc&gt;=3)
		f = creat(argv[2], 0666);
	else
		f = dup(1);
	if(f &lt; 0) {
		printf(&quot;%s: cannot open\n&quot;, argv[2]);
		exit();
	}
	fout = f;
	pass2();
	flush();
	onintr();
}

onintr()
{
	unlink(tfil);
	exit();
}

pass1()
{
	int n, c, i, ll, j, k, cc, ccc;

loop:
	if ((c=getchar())==&#39;\0&#39;)
		return;
	n = 0;
	i = 0;
	while(c!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;) {
		if(c == &#39;(&#39;)
			c = 0177;
		if(c==&#39; &#39; || c==&#39;\t&#39;) {
			i++;
			c = getchar();
			continue;
		}
		if(i) {
			i = 0;
			if(n&lt;=llen) line[n++] = &#39; &#39;;
		}
		if (n&lt;=llen) line[n++] = c;
		c = getchar();
	}
	ll = n;
	line[n++] = 0;
	i = -1;
l1:
	while((cc=line[++i])==&#39; &#39;);
	n = i;
	j = 0;
	while(sw[j]) {
		i = n;
		k = 0;
		while ((cc=sw[j][k++])==line[i++]);
		if(cc==0 &amp;&amp; ((ccc=line[--i])==&#39; &#39;||ccc==0))
			goto l1;
		j++;
	}
	i = n;
	while (c=line[n++]) putchar(c);
	putchar(&#39;~&#39;);
	n = 0;
	while (n&lt;i) {
		c = line[n++];
		if (c!=&#39; &#39; || n!=i)
			putchar(c);
	}
	putchar(&#39;\n&#39;);
	while((c=line[i++])!=0 &amp;&amp; c!=&#39; &#39;);
	--i;
	if (c) goto l1;
	goto loop;
}

pass2()
{
	int i, n, c, tilde, llen2, nbfore, nafter;


	llen2 = llen/2+6;
loop:
	if ((c=getchar())==&#39;\0&#39;)
		return;
	n = nbfore = nafter = 0;
	tilde = -1;
	while(c!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;) {
		if(c == 0177)
			c = &#39;(&#39;;
		if (n&lt;=llen) line[n] = c;
		if (c==&#39;~&#39;) tilde = n;
		if (tilde&gt;=0) nafter++; else nbfore++;
		n++;
		c = getchar();
	}
	if (tilde&lt;0)
		tilde = n++;
	nafter--;
	if (nbfore&gt;llen2) {
		i = tilde;
		while (nbfore &gt; llen2)
			while(line[--i]!=&#39; &#39; &amp;&amp; i&gt;=0) nbfore--;
		if (i&lt;0) goto l1;
		line[tilde] = 0200;
		nafter =+ (tilde-i+2);
		tilde = i;
	}
	if (nafter &gt;= llen-llen2) {
		i = tilde;
		while(nafter-- &gt;= llen-llen2)
			while(line[++i]!=&#39; &#39; &amp;&amp; i&lt;n) nafter--;
		if (i&gt;=n) goto l1;
		line[tilde] = 0200;
		nafter++;
		tilde = i;
	}
l1:
	if(!ptflg) {
		for(i=llen-llen2-nafter; i&gt;=8; i =- 8)
			putchar(&#39;\t&#39;);
		while(--i&gt;=0)
			putchar(&#39; &#39;);
	} else
		printf(&quot;.xx \&quot;&quot;);
	i = tilde;
	while (++i&lt;n) p1char(line[i]);
	if(!ptflg)
		printf(&quot;  &quot;); else
		printf(&quot;\&quot; \&quot;&quot;);
	i = -1;
	while(++i&lt;tilde) p1char(line[i]);
	if(ptflg)
		putchar(&#39;&quot;&#39;);
	putchar(&#39;\n&#39;);
	goto loop;
}

p1char(c)
{
	if ((c&amp;0377) == 0200) {
		putchar(&#39;.&#39;);
		putchar(&#39;.&#39;);
		c = &#39;.&#39;;
	}
	putchar(c);
}
