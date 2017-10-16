main(argc, argv)
char **argv;
{
	register i, m;
	register char *c;
	int count;

	if(argc &lt; 3) {
		printf(&quot;arg count\n&quot;);
		exit(1);
	}
	c = argv[1];
	m = 0;
	for(m=0; *c; c++) {
		if(*c &lt; &#39;0&#39; || *c &gt; &#39;7&#39;) {
			printf(&quot;bad mode\n&quot;);
			exit(1);
		}
		m = (m&lt;&lt;3) | *c - &#39;0&#39;;
	}
	for(i=2; i&lt;argc; i++)
		if(chmod(argv[i], m) &lt; 0) {
			count++;
			perror(argv[i]);
		}
	exit(count);
}
