char	mesg[3000];
int	msize;
struct
{
	char	name[8];
	char	tty[2];
	int	time[2];
	int	junk;
} utmp[50];

main(argc, argv)
char *argv[];
{
	register i, *p;
	int f;

	f = open(&quot;/etc/utmp&quot;, 0);
	if(f &lt; 0) {
		printf(&quot;utmp?\n&quot;);
		exit();
	}
	read(f, utmp, sizeof utmp);
	close(f);
	f = 0;
	if(argc &gt;= 2) {
		f = open(argv[1], 0);
		if(f &lt; 0) {
			printf(&quot;%s?\n&quot;, argv[1]);
			exit();
		}
	}
	while((i = read(f, &amp;mesg[msize], sizeof mesg - msize)) &gt; 0)
		msize =+ i;
	close(f);
	for(i=0; i&lt;sizeof utmp/sizeof utmp[0]; i++) {
		p = &amp;utmp[i];
		if(p-&gt;tty[0] == 0)
			continue;
		sleep(1);
		sendmes(p-&gt;tty[0]);
	}
}

sendmes(tty)
{
	register i;
	register char *s;

	i = fork();
	if(i == -1) {
		printf(&quot;try again\n&quot;);
		return;
	}
	if(i)
		return;
	s = &quot;/dev/ttyx&quot;;
	s[8] = tty;
	i = open(s, 1);
	if(i &lt; 0) {
		printf(&quot;cannot open tty%c\n&quot;, tty);
		exit();
	}
	close(1);
	dup(i);
	printf(&quot;Broadcast Message ...\n\n&quot;);
	write(1, mesg, msize);
	exit();
}
