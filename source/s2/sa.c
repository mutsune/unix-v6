#define size 2000
/* interpret command time accounting */

int	lflg;
int	cflg;
int	jflg;
int	nflg;
int	aflg;
int	rflg;
int	tflg;
int	vflg;
int	uflg;
int	thres	1;
int	sflg;
int	bflg;
int	mflg;
int	fout;

struct	user {
	int	ncomm;
	int	fill;
	float	fctime;
} user[256];
struct tab {
	char name[8];
	int count;
	float realt;
	float cput;
	float syst;
} tab[size];

struct ftab {
	char	fname[14];
	char	shf;
	char	uid;
	int	fdatet[2];
	int	frealt[2];
	int	fcput[2];
	int	fsyst[2];
};
float	treal;
float	tcpu;
float	tsys;
int	junkp -1;
char	*sname;
float	ncom;

main(argc, argv)
char **argv;
{
	int i, j, k;
	extern tcmp(), ncmp(), bcmp();
	extern float sum();
	float ft;

	init();
	if (argc&gt;1)
	if (argv[1][0]==&#39;-&#39;) {
		argv++;
		argc--;
		for(i=1; argv[0][i]; i++)
		switch(argv[0][i]) {

		case &#39;b&#39;:
			bflg++;
			break;

		case &#39;l&#39;:
			lflg++;
			break;

		case &#39;c&#39;:
			cflg++;
			break;

		case &#39;j&#39;:
			jflg++;
			break;

		case &#39;n&#39;:
			nflg++;
			break;

		case &#39;a&#39;:
			aflg++;
			break;

		case &#39;r&#39;:
			rflg++;
			break;

		case &#39;t&#39;:
			tflg++;
			break;

		case &#39;s&#39;:
			sflg++;
			aflg++;
			break;

		case &#39;0&#39;:
		case &#39;1&#39;:
		case &#39;2&#39;:
		case &#39;3&#39;:
		case &#39;4&#39;:
		case &#39;5&#39;:
		case &#39;6&#39;:
		case &#39;7&#39;:
		case &#39;8&#39;:
		case &#39;9&#39;:
			thres = argv[0][i]-&#39;0&#39;;
			break;

		case &#39;v&#39;:
			vflg++;
			break;

		case &#39;u&#39;:
			uflg++;
			break;

		case &#39;m&#39;:
			mflg++;
			break;
		}
	}
	fout = dup(1);
	if (argc&lt;2)
		acct(&quot;/usr/adm/sha&quot;);
	else while (--argc)
		acct(*++argv);
	if (uflg) {
		flush();
		return;
	}

/*
 * cleanup pass
 * put junk together
 */

	if (vflg)
		strip();
	if(!aflg)
	for (i=0; i&lt;size; i++)
	if (tab[i].name[0]) {
		for(j=0; j&lt;8; j++)
			if(tab[i].name[j] == &#39;?&#39;)
				goto yes;
		if(tab[i].count != 1)
			continue;
	yes:
		if(junkp == -1)
			junkp = enter(&quot;***other&quot;);
		tab[junkp].count =+ tab[i].count;
		tab[junkp].realt =+ tab[i].realt;
		tab[junkp].cput =+ tab[i].cput;
		tab[junkp].syst =+ tab[i].syst;
		tab[i].name[0] = 0;
	}
	for(i=k=0; i&lt;size; i++)
	if(tab[i].name[0]) {
		for(j=0; j&lt;8; j++)
			tab[k].name[j] = tab[i].name[j];
		tab[k].count = tab[i].count;
		tab[k].realt = tab[i].realt;
		tab[k].cput = tab[i].cput;
		tab[k].syst = tab[i].syst;
		k++;
	}
	if (sflg) {
		signal(2, 1);
		i = creat(&quot;/usr/adm/shm&quot;, 0666);
		write(i, user, sizeof(user));
		close(i);
		if ((i = creat(&quot;/usr/adm/sht&quot;, 0666))&lt;0) {
			printf(&quot;Can&#39;t save\n&quot;);
			exit();
		}
		write(i, tab, k*sizeof(*tab));
		close(i);
		if (sname) {
			if ((i = creat(sname, 0666))&lt;0)
				printf(&quot;Can&#39;t truncate\n&quot;);
			close(i);
		}
		signal(2, 0);
	}
/*
 * sort and print
 */

	if (mflg) {
		printmoney();
		flush();
		exit();
	}
	qsort(tab, k, 22, nflg? &amp;ncmp: (bflg?&amp;bcmp:&amp;tcmp));
	printf(&quot;%8s&quot;, &quot;&quot;);
	column(ncom, treal, tcpu, tsys);
	for (i=0; i&lt;k; i++)
	if (tab[i].name[0]) {
		ft = tab[i].count;
		printf(&quot;%-8.8s&quot;, tab[i].name);
		column(ft, tab[i].realt, tab[i].cput, tab[i].syst);
	}
	flush();
}

printmoney()
{
	register i;
	char buf[128];
	register char *cp;

	for (i=0; i&lt;256; i++) {
		if (user[i].ncomm) {
			if (getpw(i, buf)!=0)
				printf(&quot;%-8d&quot;, i);
			else {
				cp = buf;
				while (*cp!=&#39;:&#39; &amp;&amp;*cp!=&#39;\n&#39; &amp;&amp; *cp)
					cp++;
				*cp = 0;
				printf(&quot;%-8s&quot;, buf);
			}
			printf(&quot;%5l %7.2f\n&quot;,
			    user[i].ncomm, user[i].fctime/60);
		}
	}
}

column(n, a, b, c)
double n, a, b, c;
{

	printf(&quot;%6.0f&quot;, n);
	if(cflg) {
		if(n == ncom)
			printf(&quot;%7s&quot;, &quot;&quot;); else
			printf(&quot;%6.2f%%&quot;, 100.*n/ncom);
	}
	col(n, a, treal);
	if(lflg) {
		col(n, b, tcpu);
		col(n, c, tsys);
	} else
		col(n, b+c, tcpu+tsys);
	if(tflg)
		printf(&quot;%6.1f&quot;, a/(b+c));
	putchar(&#39;\n&#39;);
}

col(n, a, m)
double n, a, m;
{

	if(jflg)
		printf(&quot;%9.2f&quot;, a/(n*60.)); else
		printf(&quot;%9.2f&quot;, a/3600.);
	if(cflg) {
		if(a == m)
			printf(&quot;%7s&quot;, &quot;&quot;); else
			printf(&quot;%6.2f%%&quot;, 100.*a/m);
	}
}

acct(f)
char *f;
{
	int ff, i, j;
	float x;
	struct ftab fbuf;
	register char *cp;
	register int c;
	extern double ltod();

	if (sflg &amp;&amp; sname) {
		printf(&quot;Only 1 file with -s\n&quot;);
		exit();
	}
	if (sflg)
		sname = f;
	if ((ff=open(f, 0))&lt;0) {
		printf(&quot;Can&#39;t open %s\n&quot;, f);
		return;
	}
	while (read(ff, &amp;fbuf, sizeof(fbuf))==sizeof(fbuf)) {
		for (cp = fbuf.name; cp &lt; &amp;fbuf.name[8]; cp++) {
			c = *cp &amp; 0377;
			if (c &amp;&amp; (c &lt; &#39; &#39; || c &gt;= 0200))
				*cp = &#39;?&#39;;
		}
		if (uflg) {
			printdate(fbuf.fdatet);
			printf(&quot; %3d %.8s\n&quot;, fbuf.uid, fbuf.name);
			continue;
		}
		if (fbuf.shf==0) {
			c = fbuf.uid&amp;0377;
			user[c].ncomm++;
			user[c].fctime =+ (ltod(fbuf.fcput)+ltod(fbuf.fsyst))/60;
		}
		ncom =+ 1.0;
		i = enter(&amp;fbuf);
		tab[i].count++;
		x = ltod(fbuf.frealt);
		x =* 60.;
		tab[i].realt =+ x;
		treal =+ x;
		x = ltod(fbuf.fcput);
		tab[i].cput =+ x;
		tcpu =+ x;
		x = ltod(fbuf.fsyst);
		tab[i].syst =+ x;
		tsys =+ x;
	}
	close(ff);
}

ncmp(p1, p2)
struct tab *p1, *p2;
{

	if(p1-&gt;count == p2-&gt;count)
		return(tcmp(p1, p2));
	if(rflg)
		return(p1-&gt;count - p2-&gt;count);
	return(p2-&gt;count - p1-&gt;count);
}

bcmp(p1, p2)
struct tab *p1, *p2;
{
	float f1, f2;
	float sum();

	f1 = sum(p1)/p1-&gt;count;
	f2 = sum(p2)/p2-&gt;count;
	if(f1 &lt; f2) {
		if(rflg)
			return(-1);
		return(1);
	}
	if(f1 &gt; f2) {
		if(rflg)
			return(1);
		return(-1);
	}
	return(0);
}
tcmp(p1, p2)
struct tab *p1, *p2;
{
	extern float sum();
	float f1, f2;

	f1 = sum(p1);
	f2 = sum(p2);
	if(f1 &lt; f2) {
		if(rflg)
			return(-1);
		return(1);
	}
	if(f1 &gt; f2) {
		if(rflg)
			return(1);
		return(-1);
	}
	return(0);
}

float sum(p)
struct tab *p;
{

	if(p-&gt;name[0] == 0)
		return(0.0);
	return(
		p-&gt;cput+
		p-&gt;syst);
}

init()
{
	struct tab tbuf;
	int i, j, f;

	if ((f=open(&quot;/usr/adm/sht&quot;, 0))&lt;0)
		goto gshm;
	while (read(f, &amp;tbuf, sizeof(tbuf)) == sizeof(tbuf)) {
		i = enter(&amp;tbuf);
		ncom =+ tbuf.count;
		tab[i].count = tbuf.count;
		treal =+ tbuf.realt;
		tab[i].realt = tbuf.realt;
		tcpu =+ tbuf.cput;
		tab[i].cput = tbuf.cput;
		tsys =+ tbuf.syst;
		tab[i].syst = tbuf.syst;
	}
	close(f);
 gshm:
	if ((f=open(&quot;/usr/adm/shm&quot;, 0)) &lt; 0)
		return;
	read(f, user, sizeof(user));
	close(f);
}

enter(fbuf)
struct ftab *fbuf;
{
	int i, j;

	i = 0;
	for (j=0; j&lt;8; j++) {
		i = i*7 + fbuf-&gt;fname[j];
	}
	if(i &lt; 0)
		i = -i;
	for (i=%size; tab[i].name[0]; i = (i+1)%size) {
		for (j=0; j&lt;8; j++)
			if (tab[i].name[j]!=fbuf-&gt;fname[j])
				goto no;
		goto yes;
	no:;
	}
	for (j=0; j&lt;8; j++)
		tab[i].name[j] = fbuf-&gt;fname[j];
yes:
	return(i);
}

strip()
{
	int i, j, k, c;

	j = enter(&quot;**junk**&quot;);
	for (i = 0; i&lt;size; i++) {
		if (tab[i].name[0] &amp;&amp; tab[i].count&lt;=thres) {
			printf(&quot;%.8s--&quot;, tab[i].name);
			flush();
			if ((c=getchar())==&#39;y&#39;) {
				tab[i].name[0] = &#39;\0&#39;;
				tab[j].count =+ tab[i].count;
				tab[j].realt =+ tab[i].realt;
				tab[j].cput =+ tab[i].cput;
				tab[j].syst =+ tab[i].syst;
			}
			while (c &amp;&amp; c!=&#39;\n&#39;)
				c = getchar();
		}
	}
}

printdate(tvec)
int tvec[2];
{
	int *lt;
	int *localtime();

	lt = localtime(tvec);
	printf(&quot;%3d %c%c%c %d&quot;, lt[7], 
		pair(lt[2]), pair(lt[1]), pair(lt[0]), lt[6]);
}

pair(n)
{
	return(n/10+&#39;0&#39; | (n%10+&#39;0&#39;)&lt;&lt;8);
}
