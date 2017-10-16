#
/*
 * ln target [ new name ]
 */

struct ibuf {
	int	inum;
	int	iflags;
	char	inl;
	char	iuid;
	int	isize;
	int	iaddr[8];
	char	*ictime[2];
	char	*imtime[2];
	int	fill;
};

#define	DIR	040000
#define	FMT	060000

main(argc, argv)
char **argv;
{
	static struct ibuf statb;
	register char *np;

	if (argc&lt;2) {
		write(1, &quot;Usage: ln target [ newname ]\n&quot;, 29);
		exit(1);
	}
	if (argc==2) {
		np = argv[1];
		while(*np++);
		while (*--np!=&#39;/&#39; &amp;&amp; np&gt;argv[1]);
		np++;
		argv[2] = np;
	}
	stat(argv[1], &amp;statb);
	if ((statb.iflags&amp;FMT) == DIR) {
		write(1, &quot;No directory link\n&quot;, 18);
		exit(1);
	}
	if (link(argv[1], argv[2])&lt;0) {
		write(1, &quot;Can&#39;t link\n&quot;, 11);
		exit(1);
	}
	exit(0);
}
