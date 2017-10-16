#define	NMOUNT	16
#define	NAMSIZ	32

struct mtab {
	char	file[NAMSIZ];
	char	spec[NAMSIZ];
} mtab[NMOUNT];

main(argc, argv)
char **argv;
{
	register int ro;
	register struct mtab *mp;
	register char *np;
	int n, mf;

	mf = open(&quot;/etc/mtab&quot;, 0);
	read(mf, mtab, NMOUNT*2*NAMSIZ);
	if (argc==1) {
		for (mp = mtab; mp &lt; &amp;mtab[NMOUNT]; mp++)
			if (mp-&gt;file[0])
				printf(&quot;%s on %s\n&quot;, mp-&gt;spec, mp-&gt;file);
		return;
	}
	if(argc &lt; 3) {
		printf(&quot;arg count\n&quot;);
		return;
	}
	ro = 0;
	if(argc &gt; 3)
		ro++;
	if(mount(argv[1], argv[2], ro) &lt; 0) {
		perror(&quot;mount&quot;);
		return;
	}
	np = argv[1];
	while(*np++)
		;
	np--;
	while(*--np == &#39;/&#39;)
		*np = &#39;\0&#39;;
	while(np &gt; argv[1] &amp;&amp; *--np != &#39;/&#39;)
		;
	if(*np == &#39;/&#39;)
		np++;
	argv[1] = np;
	for (mp = mtab; mp &lt; &amp;mtab[NMOUNT]; mp++) {
		if (mp-&gt;file[0] == 0) {
			for (np = mp-&gt;spec; np &lt; &amp;mp-&gt;spec[NAMSIZ-1];)
				if ((*np++ = *argv[1]++) == 0)
					argv[1]--;
			for (np = mp-&gt;file; np &lt; &amp;mp-&gt;file[NAMSIZ-1];)
				if ((*np++ = *argv[2]++) == 0)
					argv[2]--;
			mp = &amp;mtab[NMOUNT];
			while ((--mp)-&gt;file[0] == 0);
			mf = creat(&quot;/etc/mtab&quot;, 0644);
			write(mf, mtab, (mp-mtab+1)*2*NAMSIZ);
			return;
		}
	}
}
