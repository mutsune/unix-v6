#define	tabsize	20
#define	all	p = &amp;itab[0]; p &lt; &amp;itab[20]; p++
#define	ever	;;
#define	single	0173030
#define	reboot	0173040
char	shell[]	&quot;/bin/sh&quot;;
char	minus[]	&quot;-&quot;;
char	runc[]	&quot;/etc/rc&quot;;
char	init[]	&quot;/etc/init&quot;;
char	ifile[]	&quot;/etc/ttys&quot;;
char	utmp[]	&quot;/etc/utmp&quot;;
char	wtmpf[]	&quot;/usr/adm/wtmp&quot;;
char	ctty[]	&quot;/dev/tty8&quot;;
int	fi;
struct
{
	int	flag;
	int	line;
	char	coms[2];
} line;
struct	tab
{
	int	pid;
	int	line;
	int	comn;
} itab[tabsize];

struct {
	char	name[8];
	char	tty;
	char	fill;
	int	time[2];
	int	wfill;
} wtmp;

main()
{
	register i;
	register struct tab *p, *q;
	int reset();

	/*
	 * if not single user,
	 * run shell sequence
	 */

	if(getcsw() != single) {
		i = fork();
		if(i == 0) {
			open(&quot;/&quot;, 0);
			dup(0);
			dup(0);
			execl(shell, shell, runc, 0);
			exit();
		}
		while(wait() != i);
		close(creat(utmp, 0644));
		if ((i = open(wtmpf, 1)) &gt;= 0) {
			seek(i, 0, 2);
			wtmp.tty = &#39;~&#39;;
			time(wtmp.time);
			write(i, &amp;wtmp, 16);
			close(i);
		}
	}

	/*
	 * main loop for hangup signal
	 * close all files and
	 * check switches for magic values
	 */

	setexit();
	signal(1, reset);
	for(i=0; i&lt;10; i++)
		close(i);
	switch(getcsw()) {

	case single:
	error:
		termall();
		i = fork();
		if(i == 0) {
			open(ctty, 2);
			dup(0);
			execl(shell, minus, 0);
			exit();
		}
		while(wait() != i);

	case reboot:
		termall();
		execl(init, minus, 0);
		reset();
	}

	/*
	 * open and merge in init file
	 */

	fi = open(ifile, 0);
	q = &amp;itab[0];
	while(rline()) {
		if(line.flag == &#39;0&#39;)
			continue;
		for(all)
			if(p-&gt;line==line.line || p-&gt;line==0) {
				if(p &gt;= q) {
					i = p-&gt;pid;
					p-&gt;pid = q-&gt;pid;
					q-&gt;pid = i;
					p-&gt;line = q-&gt;line;
					p-&gt;comn = q-&gt;comn;
					q-&gt;line = line.line;
					q-&gt;coms[0] = line.comn;
					q++;
				}
				break;
			}
	}
	close(fi);
	if(q == &amp;itab[0])
		goto error;
	for(; q &lt; &amp;itab[tabsize]; q++)
		term(q);
	for(all)
		if(p-&gt;line != 0 &amp;&amp; p-&gt;pid == 0)
			dfork(p);
	for(ever) {
		i = wait();
		for(all)
			if(p-&gt;pid == i) {
				rmut(p);
				dfork(p);
			}
	}
}

termall()
{
	register struct tab *p;

	for(all)
		term(p);
}

term(ap)
struct tab *ap;
{
	register struct tab *p;

	p = ap;
	if(p-&gt;pid != 0) {
		rmut(p);
		kill(p-&gt;pid, 9);
	}
	p-&gt;pid = 0;
	p-&gt;line = 0;
}

rline()
{
	static char c[4];

	if(read(fi, c, 4) != 4 || c[3] != &#39;\n&#39;)
		return(0);
	line.flag = c[0];
	line.line = c[1];
	line.comn = c[2];
	return(1);
}

dfork(ap)
struct tab *ap;
{
	register i;
	register char *tty;
	register struct tab *p;

	p = ap;
	i = fork();
	if(i == 0) {
		signal(1, 0);
		tty = &quot;/dev/ttyx&quot;;
		tty[8] = p-&gt;line;
		chown(tty, 0);
		chmod(tty, 0622);
		open(tty, 2);
		dup(0);
		execl(&quot;etc/getty&quot;, minus, p-&gt;coms, 0);
		exit();
	}
	p-&gt;pid = i;
}

rmut(p)
struct tab *p;
{
	register i, f;
	static char zero[16];

	f = open(utmp, 1);
	if(f &gt;= 0) {
		i = p-&gt;line;
		if(i &gt;= &#39;a&#39;)
			i =+ &#39;0&#39; + 10 - &#39;a&#39;;
		seek(f, (i-&#39;0&#39;)*16, 0);
		write(f, zero, 16);
		close(f);
	}
	f = open(wtmpf, 1);
	if (f &gt;= 0) {
		wtmp.tty = p-&gt;line;
		time(wtmp.time);
		seek(f, 0, 2);
		write(f, &amp;wtmp, 16);
		close(f);
	}
}
