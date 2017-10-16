int	count	1000;
int	fnumber;
int	ibuf[259];
int	obuf[259];
char	fname[100];
char	*ifil;
char	*ofil;

main(argc, argv)
char *argv[];
{
	register i, c, f;

	for(i=1; i&lt;argc; i++)
	if(argv[i][0] == &#39;-&#39;)
	switch(argv[i][1]) {

	case &#39;\0&#39;:
		ifil = 1;
		continue;

	case &#39;0&#39;:
	case &#39;1&#39;:
	case &#39;2&#39;:
	case &#39;3&#39;:
	case &#39;4&#39;:
	case &#39;5&#39;:
	case &#39;6&#39;:
	case &#39;7&#39;:
	case &#39;8&#39;:
	case &#39;9&#39;:
		count = number(argv[i]+1);
		continue;

	} else
	if(ifil)
		ofil = argv[i]; else
		ifil = argv[i];
	if(ifil != 0 &amp;&amp; ifil != 1)
		if(fopen(ifil, ibuf) &lt; 0) {
			write(2, &quot;cannot open input\n&quot;, 18);
			exit();
		}
	if(ofil == 0)
		ofil = &quot;x&quot;;

loop:
	f = 1;
	for(i=0; i&lt;count; i++)
	do {
		c = getc(ibuf);
		if(c &lt; 0) {
			if(f == 0)
				fflush(obuf);
			exit();
		}
		if(f) {
			for(f=0; ofil[f]; f++)
				fname[f] = ofil[f];
			fname[f++] = fnumber/26 + &#39;a&#39;;
			fname[f++] = fnumber%26 + &#39;a&#39;;
			fname[f] = &#39;\0&#39;;
			fnumber++;
			if(fcreat(fname, obuf) &lt; 0) {
				write(2, &quot;Cannot create output\n&quot;, 20);
				exit();
			}
			f = 0;
		}
		putc(c, obuf);
	} while(c != &#39;\n&#39;);
	fflush(obuf);
	close(obuf[0]);
	goto loop;
}

number(str)
char *str;
{
	register n;
	register char *s;

	n = 0;
	for(s = str; *s; s++)
		if(*s&gt;=&#39;0&#39; &amp;&amp; *s&lt;=&#39;9&#39;)
			n = n*10 + *s-&#39;0&#39;;
	return(n);
}
