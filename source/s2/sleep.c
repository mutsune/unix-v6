main(argc, argv)
char **argv;
{
	int c, n;
	char *s;

	n = 0;
	if(argc &lt; 2) {
		printf(&quot;arg count\n&quot;);
		exit();
	}
	s = argv[1];
	while(c = *s++) {
		if(c&lt;&#39;0&#39; || c&gt;&#39;9&#39;) {
			printf(&quot;bad character\n&quot;);
			exit();
		}
		n = n*10 + c - &#39;0&#39;;
	}
	sleep(n);
}
