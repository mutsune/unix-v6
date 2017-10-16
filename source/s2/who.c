/*
 * who
 */

int	fout;
int	buf[256];

main(argc, argv)
char **argv;
{
	char *s, *cbuf;
	int n, fi, i;
	int tty;
	struct {
		char name[8];
		char tty;
		char pad1;
		int time[2];
		char pad2[2];
	} *p;

	s = &quot;/etc/utmp&quot;;
	if(argc == 2)
		s = argv[1];
	fi = open(s, 0);
	if(fi &lt; 0) {
		write(&quot;cannot open wtmp\n&quot;, 17);
		exit();
	}
	fout = dup(1);
	close(1);
	if (argc==3)
		tty = ttyn(0);

loop:
	n = read(fi, buf, 512);
	if(n == 0) {
		flush();
		if (argc==3)
			write(fout, &quot;Nobody.\n&quot;, 8);
		exit();
	}

	p = &amp;buf;
	for(p = &amp;buf; (n =- 16)&gt;=0; p++) {
		if (argc==3 &amp;&amp; tty!=p-&gt;tty)
			continue;
		if(p-&gt;name[0] == &#39;\0&#39; &amp;&amp; argc==1)
			continue;
		for(i=0; i&lt;8; i++) {
			if(p-&gt;name[i] == &#39;\0&#39;)
				p-&gt;name[i] = &#39; &#39;;
			putchar(p-&gt;name[i]);
		}
		for(i=0; i&lt;3; i++)
			putchar(&quot;tty&quot;[i]);
		putchar(p-&gt;tty);
		cbuf = ctime(p-&gt;time);
		for(i=3; i&lt;16; i++)
			putchar(cbuf[i]);
		putchar(&#39;\n&#39;);
		if (argc==3) {
			flush();
			exit();
		}
	}
	goto loop;
}
