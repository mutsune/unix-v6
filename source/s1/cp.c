/*
 * cp oldfile newfile
 */

main(argc,argv)
char **argv;
{
	static int buf[256];
	int fold, fnew, n;
	register char *p1, *p2, *bp;
	int mode;

	if(argc != 3) {
		write(1, &quot;Usage: cp oldfile newfile\n&quot;, 26);
		exit(1);
	}
	if((fold = open(argv[1], 0)) &lt; 0) {
		write(1, &quot;Cannot open old file.\n&quot;, 22);
		exit(1);
	}
	fstat(fold, buf);
	mode = buf[2];
	/* is target a directory? */
	if (stat(argv[2], buf+50)&gt;=0 &amp;&amp; (buf[52]&amp;060000)==040000) {
		p1 = argv[1];
		p2 = argv[2];
		bp = buf+100;
		while(*bp++ = *p2++);
		bp[-1] = &#39;/&#39;;
		p2 = bp;
		while(*bp = *p1++)
			if(*bp++ == &#39;/&#39;)
				bp = p2;
		argv[2] = buf+100;
	}
	if (stat(argv[2], buf+50) &gt;= 0) {
		if (buf[0]==buf[50] &amp;&amp; buf[1]==buf[51]) {
			write(1, &quot;Copying file to itself.\n&quot;, 24);
			exit(1);
		}
	}
	if ((fnew = creat(argv[2], mode)) &lt; 0) {
		write(1, &quot;Can&#39;t create new file.\n&quot;, 23);
		exit(1);
	}
	while(n = read(fold,  buf,  512)) {
	if(n &lt; 0) {
		write(1, &quot;Read error\n&quot;, 11);
		exit(1);
	} else
		if(write(fnew, buf, n) != n){
			write(1, &quot;Write error.\n&quot;, 13);
			exit(1);
		}
	}
	exit(0);
}
