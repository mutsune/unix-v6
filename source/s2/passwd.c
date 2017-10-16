/*
 * enter a password in the password file
 * this program should be suid with owner
 * with an owner with write permission on /etc/passwd
 */
char	*tfile	{ &quot;/tmp/ptmp&quot; };
char	*pfile	{ &quot;/etc/passwd&quot; };
int	tbuf[259];
int	pbuf[259];

main(argc, argv)
char *argv[];
{
	register u, c;
	register char *p;

	if(argc != 3) {
		write(2, &quot;Usage: passwd user password\n&quot;, 28);
		goto bex;
	}
	signal(1, 1);
	signal(2, 1);
	signal(3, 1);

	if(stat(tfile, tbuf+20) &gt;= 0) {
		write(2, &quot;Temporary file busy -- try again\n&quot;, 33);
		goto bex;
	}
	tbuf[0] = creat(tfile, 0600);
	if(tbuf[0] &lt; 0) {
		write(2, &quot;Cannot create temporary file\n&quot;, 29);
		goto bex;
	}
	pbuf[0] = open(pfile, 0);
	if(pbuf[0] &lt; 0) {
		write(2, &quot;Cannot open /etc/passwd\n&quot;, 24);
		goto out;
	}
	goto l1;

/*
 * skip to beginning of next line
 */

skip:
	while(c != &#39;\n&#39;) {
		if(c &lt; 0)
			goto ill;
		c = getc(pbuf);
		putc(c, tbuf);
	}

/*
 * compare user names
 */

l1:
	c = getc(pbuf);
	putc(c, tbuf);
	if(c &lt; 0) {
		write(2, &quot;User name not found in password file\n&quot;, 37);
		goto out;
	}
	p = argv[1];
	while(c != &#39;:&#39;) {
		if(*p++ != c)
			goto skip;
		c = getc(pbuf);
		putc(c, tbuf);
	}
	if(*p)
		goto skip;
/*
 * skip old password
 */
	do {
		c = getc(pbuf);
		if(c &lt; 0)
			goto ill;
	} while(c != &#39;:&#39;);

/*
 * copy in new password
 */
	p = argv[2];
	for(c=0; c&lt;9; c++)
		if(*p++ == 0)
			break;
	*--p = 0;
	if(p != argv[2])
		p = crypt(argv[2]);
	while(*p)
		putc(*p++, tbuf);
	putc(&#39;:&#39;, tbuf);

/*
 * validate uid
 */

	u = 0;
	do {
		c = getc(pbuf);
		putc(c, tbuf);
		if(c &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;)
			u = u*10 + c-&#39;0&#39;;
		if(c &lt; 0)
			goto ill;
	} while(c != &#39;:&#39;);
	c = getuid() &amp; 0377;
	if(c != 0 &amp;&amp; c != u) {
		write(2, &quot;Permission denied\n&quot;, 18);
		goto out;
	}

/*
 * copy out and back
 */

	for(;;) {
		c = getc(pbuf);
		if(c &lt; 0) {
			fflush(tbuf);
			close(pbuf[0]);
			close(tbuf[0]);
			tbuf[0] = open(tfile, 0);
			if(tbuf[0] &lt; 0) {
				write(2, &quot;Urk\n&quot;, 4);
				goto out;
			}
			pbuf[0] = creat(pfile, 0644);
			if(pbuf[0] &lt; 0) {
				write(2, &quot;Cannot create /etc/passwd\n&quot;, 26);
				goto out;
			}
			while((c = read(tbuf[0], tbuf+1, 512)) &gt; 0)
				write(pbuf[0], tbuf+1, c);
			unlink(tfile);
			exit(0);
		}
		putc(c, tbuf);
	}

ill:
	write(2, &quot;Password file illformed\n&quot;, 24);

out:
	unlink(tfile);

bex:
	exit(1);
}
