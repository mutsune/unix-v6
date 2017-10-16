char	buf[100];

main(argc, argv)
char **argv;
{
	register i;
	register char *c1, *c2;

	if(argc &lt; 3) {
		write(2, &quot;arg count\n&quot;, 10);
		exit();
	}
	argc--;
	c1 = buf;
	c2 = argv[argc];
	while(*c1++ = *c2++);
	c1[-1] = &#39;/&#39;;
	*c1++ = &#39;.&#39;;
	*c1 = &#39;\0&#39;;
	for(i=1; i&lt;argc; i++) {
		if(fork()==0) {
			execl(&quot;/bin/cp&quot;, &quot;cp&quot;, argv[i], buf);
			exit();
		}
		wait();
	}
}
