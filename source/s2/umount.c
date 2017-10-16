#define	NMOUNT	16
#define	NAMSIZ	32

struct mtab {
	char	file[NAMSIZ];
	char	spec[NAMSIZ];
} mtab[NMOUNT];

main(argc, argv)
char **argv;
{
	register struct mtab *mp;
	register char *p1, *p2;
	int mf;

	sync();
	mf = open(&quot;/etc/mtab&quot;, 0);
	read(mf, mtab, NMOUNT*2*NAMSIZ);
	if(argc != 2) {
		printf(&quot;arg count\n&quot;);
		return;
	}
	if (umount(argv[1]) &lt; 0) {
		perror(&quot;umount&quot;);
		return;
	}
	p1 = argv[1];
	while(*p1++)
		;
	p1--;
	while(*--p1 == &#39;/&#39;)
		*p1 = &#39;\0&#39;;
	while(p1 &gt; argv[1] &amp;&amp; *--p1 != &#39;/&#39;)
		;
	if(*p1 == &#39;/&#39;)
		p1++;
	argv[1] = p1;
	for (mp = mtab; mp &lt; &amp;mtab[NMOUNT]; mp++) {
		p1 = argv[1];
		p2 = &amp;mp-&gt;spec[0];
		while (*p1++ == *p2)
			if (*p2++ == 0) {
				for (p1 = mp-&gt;file; p1 &lt; &amp;mp-&gt;file[NAMSIZ*2];)
					*p1++ = 0;
				mp = &amp;mtab[NMOUNT];
				while ((--mp)-&gt;file[0] == 0);
				mf = creat(&quot;/etc/mtab&quot;, 0644);
				write(mf, mtab, (mp-mtab+1)*2*NAMSIZ);
				return;
			}
	}
	printf(&quot;%s not in mount table\n&quot;, argv[1]);
}
