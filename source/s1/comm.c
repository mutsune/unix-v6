#define LB 250
int	one;
int	two;
int	three;

char	*ldr[3];

char	ib1[518];
char	ib2[518];
main(argc,argv)
	char	*argv[];
{
	extern	fout;
	int	l;
	char	lb1[LB],lb2[80];

	ldr[0] = &quot;&quot;;
	ldr[1] = &quot;\t&quot;;
	ldr[2] = &quot;\t\t&quot;;
	if(argc &gt; 1)  {
		if(*argv[1] == &#39;-&#39; &amp;&amp; argv[1][1] != 0) {
			l = 1;
			while(*++argv[1]) {
				switch(*argv[1]) {
				case&#39;1&#39;:
					if(!one) {
						one = 1;
						ldr[1][0] =
						ldr[2][l--] = &#39;\0&#39;;
					}
					break;
				case &#39;2&#39;:
					if(!two) {
						two = 1;
						ldr[2][l--] = &#39;\0&#39;;
					}
					break;
				case &#39;3&#39;:
					three = 1;
					break;
				default:
				printf(&quot;Illegal flag: %c\n&quot;,*argv[1]);
				exit();
				}
			}
			argv++;
			argc--;
		}
	}

	if(argc &lt; 3) {
		printf(&quot;Argc = %d\n&quot;,argc);
		exit();
	}

	openfil(argv[1],ib1);
	openfil(argv[2],ib2);
	fout = dup(1);


	if(rd(ib1,lb1) &lt; 0) {
		if(rd(ib2,lb2) &lt; 0)	exit();
		copy(ib2,lb2,2);
	}
	if(rd(ib2,lb2) &lt; 0)	copy(ib1,lb1,1);

	while(1) {

		switch(compare(lb1,lb2)) {

			case 0:
				wr(lb1,3);
				if(rd(ib1,lb1) &lt; 0) {
					if(rd(ib2,lb2) &lt; 0)	fexit();
					copy(ib2,lb2,2);
				}
				if(rd(ib2,lb2) &lt; 0)	copy(ib1,lb1,1);
				continue;

			case 1:
				wr(lb1,1);
				if(rd(ib1,lb1) &lt; 0)	copy(ib2,lb2,2);
				continue;

			case 2:
				wr(lb2,2);
				if(rd(ib2,lb2) &lt; 0)	copy(ib1,lb1,1);
				continue;
		}
	}
}

rd(file,buf)
	char	*buf;
{

	register int i;
	i = 0;
	while((*buf = getc(file)) &gt;=0) {
		if(*buf == &#39;\n&#39; || i &gt; LB-2) {
			*buf = &#39;\0&#39;;
			return(0);
		}
		i++;
		buf++;
	}
	return(-1);
}

wr(str,n)
	char	*str;
{

	switch(n) {

		case 1:
			if(one)	return;
			break;

		case 2:
			if(two)	return;
			break;

		case 3:
			if(three)	return;
	}
	printf(&quot;%s%s\n&quot;,ldr[n-1],str);
}

copy(ibuf,lbuf,n)
{
	do {
		wr(lbuf,n);
	} while(rd(ibuf,lbuf) &gt;= 0);

	flush();
	exit();
}

compare(a,b)
	char	*a,*b;
{
	register char *ra,*rb;

	ra = --a;
	rb = --b;
	while(*++ra == *++rb)
		if(*ra == &#39;\0&#39;)	return(0);
	if(*ra &lt; *rb)	return(1);
	return(2);
}
fexit()
{
	flush();
	exit();
}

openfil(s,b)
char *s;
int *b;
{
	if(s[0]==&#39;-&#39; &amp;&amp; s[1]==0)
		b[0] = 0;
	else if(fopen(s, b) == -1) {
		printf(&quot;Can&#39;t open %s\n&quot;, s);
		exit(1);
	}
}
