int dflag 0;
int sflag 0;
int cflag 0;
int save 0;
char code[256];
char squeez[256];
char vect[256];
struct string { int last, max, rep; char *p; } string1, string2;
int inbuf[259];

main(argc,argv)
char **argv;
{
	int i, j;
	int c, d;
	char *compl;
	extern fout;

	string1.last = string2.last = 0;
	string1.max = string2.max = 0;
	string1.rep = string2.rep = 0;
	string1.p = string2.p = &quot;&quot;;

	if(--argc&gt;0) {
		argv++;
		if(*argv[0]==&#39;-&#39;&amp;&amp;argv[0][1]!=0) {
			while(*++argv[0])
				switch(*argv[0]) {
				case &#39;c&#39;:
					cflag++;
					continue;
				case &#39;d&#39;:
					dflag++;
					continue;
				case &#39;s&#39;:
					sflag++;
					continue;
				}
			argc--;
			argv++;
		}
	}
	if(argc&gt;0) string1.p = argv[0];
	if(argc&gt;1) string2.p = argv[1];
	for(i=0; i&lt;256; i++)
		code[i] = vect[i] = 0;
	if(cflag) {
		while(c = next(&amp;string1))
			vect[c&amp;0377] = 1;
		j = 0;
		for(i=1; i&lt;256; i++)
			if(vect[i]==0) vect[j++] = i;
		vect[j] = 0;
		compl = vect;
	}
	for(i=0; i&lt;256; i++)
		squeez[i] = 0;
	for(;;){
		if(cflag) c = *compl++;
		else c = next(&amp;string1);
		if(c==0) break;
		d = next(&amp;string2);
		if(d==0) d = c;
		code[c&amp;0377] = d;
		squeez[d&amp;0377] = 1;
	}
	while(d = next(&amp;string2))
		squeez[d&amp;0377] = 1;
	squeez[0] = 1;
	for(i=0;i&lt;256;i++) {
		if(code[i]==0) code[i] = i;
		else if(dflag) code[i] = 0;
	}

	inbuf[0] = 0;
	fout = dup(1);
	close(1);
	while((c=getc(inbuf)) &gt;=0 ) {
		if(c == 0) continue;
		if(c = code[c&amp;0377]&amp;0377)
			if(!sflag || c!=save || !squeez[c&amp;0377])
				putchar(save = c);
	}
	flush();
}

next(s)
struct string *s;
{
	int a, b, c, n;
	int base;

	if(--s-&gt;rep &gt; 0) return(s-&gt;last);
	if(s-&gt;last &lt; s-&gt;max) return(++s-&gt;last);
	if(*s-&gt;p==&#39;[&#39;) {
		nextc(s);
		s-&gt;last = a = nextc(s);
		s-&gt;max = 0;
		switch(nextc(s)) {
		case &#39;-&#39;:
			b = nextc(s);
			if(b&lt;a || *s-&gt;p++!=&#39;]&#39;)
				goto error;
			s-&gt;max = b;
			return(a);
		case &#39;*&#39;:
			base = (*s-&gt;p==&#39;0&#39;)?8:10;
			n = 0;
			while((c = *s-&gt;p)&gt;=&#39;0&#39; &amp;&amp; c&lt;&#39;0&#39;+base) {
				n = base*n + c - &#39;0&#39;;
				s-&gt;p++;
			}
			if(*s-&gt;p++!=&#39;]&#39;) goto error;
			if(n==0) n = 1000;
			s-&gt;rep = n;
			return(a);
		default:
		error:
			write(1,&quot;Bad string\n&quot;,11);
			exit();
		}
	}
	return(nextc(s));
}

nextc(s)
struct string *s;
{
	int c, i, n;

	c = *s-&gt;p++;
	if(c==&#39;\\&#39;) {
		i = n = 0;
		while(i&lt;3 &amp;&amp; (c = *s-&gt;p)&gt;=&#39;0&#39; &amp;&amp; c&lt;=&#39;7&#39;) {
			n = n*8 + c - &#39;0&#39;;
			i++;
			s-&gt;p++;
		}
		if(i&gt;0) c = n;
		else c = *s-&gt;p++;
	}
	if(c==0) *--s-&gt;p = 0;
	return(c&amp;0377);
}
