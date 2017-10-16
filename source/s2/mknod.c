main(argc, argv)
int argc;
char **argv;
{
	int m, a, b;

	if(argc != 5) {
		printf(&quot;arg count\n&quot;);
		goto usage;
	}
	if(*argv[2] == &#39;b&#39;)
		m = 0160666; else
	if(*argv[2] == &#39;c&#39;)
		m = 0120666; else
		goto usage;
	a = number(argv[3]);
	if(a &lt; 0)
		goto usage;
	b = number(argv[4]);
	if(b &lt; 0)
		goto usage;
	if(mknod(argv[1], m, (a&lt;&lt;8)|b) &lt; 0)
		perror(&quot;mknod&quot;);
	exit();

usage:
	printf(&quot;usage: mknod name b/c major minor\n&quot;);
}

number(s)
char *s;
{
	int n, c;

	n = 0;
	while(c = *s++) {
		if(c&lt;&#39;0&#39; || c&gt;&#39;9&#39;)
			return(-1);
		n = n*10 + c-&#39;0&#39;;
	}
	return(n);
}
