/* nice */
int	nicarg	4;
char	*args[100];
char	string[10000];

main(argc, argv)
int argc;
char *argv[];
{
	int i;
	register char **argp, *strp, *p;

	if(argc &gt; 1 &amp;&amp; argv[1][0] == &#39;-&#39;) {
		nicarg = atoi(&amp;argv[1][1]);
		argc--;
		argv++;
	}
	if(argc &lt; 2) {
		printf(&quot;usage: nice [ -n ] command\n&quot;);
		exit(1);
	}
	argc--;
	argv++;
	argp = args;
	strp = string;
	for (i=0; i&lt;9; i++)
		*strp++ = &quot;/usr/bin/&quot;[i];
	for(i=0; i&lt;argc; i++) {
		*argp++ = strp;
		p = *argv++;
		while(*strp++ = *p++);
	}
	*argp = 0;
	nice(nicarg);
	execv(string+9, args);
	execv(string+4, args);
	execv(string, args);
	printf(&quot;%s not found\n&quot;, string+9);
	exit(1);
}
