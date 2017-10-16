char	*dargv[]
{
	0,
	&quot;/dev/rk2&quot;,
	&quot;/dev/rp0&quot;,
	0
};
struct
{
	char	*s_isize;
	char	*s_fsize;
	int	s_nfree;
	int	s_free[100];
	int	s_ninode;
	int	s_inode[100];
	char	s_flock;
	char	s_ilock;
	char	s_fmod;
	int	time[2];
	int	pad[50];
} sblock;

int	fi;

main(argc, argv)
char **argv;
{
	int i;

	if(argc &lt;= 1) {
		for(argc = 1; dargv[argc]; argc++);
		argv = dargv;
	}

	for(i=1; i&lt;argc; i++) {
		if(argc &gt; 1)
			printf(&quot;%s &quot;, argv[i]);
		dfree(argv[i]);
	}
}

dfree(file)
char *file;
{
	int i;

	fi = open(file, 0);
	if(fi &lt; 0) {
		printf(&quot;cannot open %s\n&quot;, file);
		return;
	}
	sync();
	bread(1, &amp;sblock);
	i = 0;
	while(alloc())
		i++;
	printf(&quot;%l\n&quot;, i);
	close(fi);
}

alloc()
{
	int b, i, buf[256];

	i = --sblock.s_nfree;
	if(i&lt;0 || i&gt;=100) {
		printf(&quot;bad free count\n&quot;);
		return(0);
	}
	b = sblock.s_free[i];
	if(b == 0)
		return(0);
	if(b&lt;sblock.s_isize+2 || b&gt;=sblock.s_fsize) {
		printf(&quot;bad free block (%l)\n&quot;, b);
		return(0);
	}
	if(sblock.s_nfree &lt;= 0) {
		bread(b, buf);
		sblock.s_nfree = buf[0];
		for(i=0; i&lt;100; i++)
			sblock.s_free[i] = buf[i+1];
	}
	return(b);
}

bread(bno, buf)
{
	int n;
	extern errno;

	seek(fi, bno, 3);
	if((n=read(fi, buf, 512)) != 512) {
		printf(&quot;read error %d\n&quot;, bno);
		printf(&quot;count = %d; errno = %d\n&quot;, n, errno);
		exit();
	}
}
