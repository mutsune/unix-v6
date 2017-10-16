/* wc line and word count */

int	buf[259];
int wordct[2];
int twordct[2];
int linect[2];
int tlinect[2];

main(argc,argv)
char **argv;
{
	int i, token;
	register char *p1, *p2;
	register int c;

	i = 1;
	do {
		if(argc&lt;=1) buf[0] = 0;
		else if(fopen(argv[i],buf)&lt;0) {
			diag(argv[i]);
			diag(&quot;: cannot open\n&quot;);
			continue;
		}
		p1 = 0;
		p2 = 0;
		linect[0] = linect[1] = 0;
		wordct[0] = wordct[1] = 0;
		token = 0;
		for(;;) {
			if(p1 &gt;= p2) {
				p1 = &amp;buf[1];
				c = read(buf[0], p1, 512);
				if(c &lt;= 0)
					break;
				p2 = p1+c;
			}
			c = 0;
			c =| *p1++;
			if(&#39; &#39;&lt;c&amp;&amp;c&lt;0177) {
				if(!token++) {
					if(++wordct[1]==0)
						wordct[0]++;
				}
			} else {
				if(c==&#39;\n&#39;) {
					if(++linect[1]==0)
						linect[0]++;
				}
				else if(c!=&#39; &#39;&amp;&amp;c!=&#39;\t&#39;)
					continue;
				token = 0;
			}
		}
		printf(&quot;%7s &quot;,locv(linect[0],linect[1]));
		printf(&quot;%7s &quot;,locv(wordct[0],wordct[1]));
		printf(&quot;%s\n&quot;, argc&lt;=1?&quot;&quot;:argv[i]);
		close(buf[0]);
		ladd(tlinect, tlinect, linect);
		ladd(twordct, twordct, wordct);
	} while(++i&lt;argc);
	if(argc &gt; 2) {
		printf(&quot;%7s &quot;,locv(tlinect[0],tlinect[1]));
		printf(&quot;%7s &quot;,locv(twordct[0],twordct[1]));
		printf(&quot;total\n&quot;);
	}
}

diag(s)
char *s;
{
	while(*s)
		write(2,s++,1);
}
