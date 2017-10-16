main(argc, argv)
char *argv[];
{
	char *arg;
	int fflg, rflg;

	fflg = 0;
	rflg = 0;
	while(--argc &gt; 0) {

		arg = *++argv;
		if(arg[0] == &#39;-&#39;) {
			if(arg[1] == &#39;f&#39;) {
				fflg++;
				continue;
			}
			if(arg[1] == &#39;r&#39;) {
				rflg++;
				continue;
			}
		}

		rm(arg, fflg, rflg);
	}
}

struct stbuf	{
	int dev;
	int inum;
	int mode;
	char nlink;
	char uid;
	char gid;
	char siz0;
	char siz1;
	int addr[8];
	int adate[2];
	int mdate[2];
	};

rm(arg, fflg, rflg)
char arg[];
{
	char *p;
	int buf[20];
	int i, b;

	if(stat(arg, buf)) {
		printf(&quot;%s: non existent\n&quot;, arg);
		return;
	}
	if((buf-&gt;mode &amp; 060000) == 040000)	{
		if(rflg) {
			i = fork();
			if(i == -1) {
				printf(&quot;%s: try again\n&quot;, arg);
				return;
			}
			if(i) {
				while(wait() != i);
				return;
			}
			if(chdir(arg)) {
				printf(&quot;%s: cannot chdir\n&quot;, arg);
				exit();
			}
			p = 0;
			execl(&quot;/etc/glob&quot;, &quot;glob&quot;, &quot;rm&quot;, &quot;-r&quot;,
				fflg? &quot;-f&quot;: &quot;*&quot;, fflg? &quot;*&quot;: p, 0);
			printf(&quot;%s: no glob\n&quot;, arg);
			exit();
		}
		printf(&quot;%s: directory\n&quot;, arg);
		return;
	}

	if(!fflg) {

		if((getuid()&amp;0377) == buf-&gt;uid)
			b = 0200; else
			b = 2;
		if((buf-&gt;mode &amp; b) == 0 &amp;&amp; ttyn(0) != &#39;x&#39;) {
			printf(&quot;%s: %o mode &quot;, arg, buf-&gt;mode);
			i = b = getchar();
			i = b;
			while(b != &#39;\n&#39; &amp;&amp; b != &#39;\0&#39;)
				b = getchar();
			if(i != &#39;y&#39;)
				return;
		}
	}
	if(unlink(arg))
		printf(&quot;%s: not removed\n&quot;, arg);
}

putchar(c)
{
	write(1, &amp;c, 1);
}

getchar()
{
	char c;

	if(read(0, &amp;c, 1) != 1) return(0);
	return(c);
}
