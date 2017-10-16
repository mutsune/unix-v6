int	fout;
int	cflg;
int	nflg;
int	uflg;
int	rflg	1;
int	gflg;
int	pflg;
struct	nl
{
	char	name[8];
	int	typ;
	int	*val;
} *nlp;
int	fi;
int	buf[8];
main(argc, argv)
char **argv;
{
	int n, i, j;
	int compare();

	if (--argc &gt; 0 &amp;&amp; *argv[1] == &#39;-&#39;) {
		argv++;
		while (*++*argv) switch (**argv) {
		case &#39;n&#39;:
			nflg++;
			continue;

		case &#39;c&#39;:
			cflg++;
			continue;

		case &#39;g&#39;:
			gflg++;
			continue;

		case &#39;u&#39;:
			uflg++;
			continue;

		case &#39;r&#39;:
			rflg = -1;
			continue;

		case &#39;p&#39;:
			pflg ++;
			continue;

		default:
			continue;
		}
		argc--;
	}
	if (argc==0)
		fi = open(&quot;a.out&quot;, 0); else
		fi = open(*++argv, 0);
	if(fi &lt; 0) {
		printf(&quot;cannot open input\n&quot;);
		exit();
	}
	read(fi, buf, 020);
	if(buf[0]!=0407 &amp;&amp; buf[0]!=0410 &amp;&amp; buf[0]!=0411) {
		printf(&quot;bad format\n&quot;);
		exit();
	}
	seek(fi, buf[1], 1);		/* text */
	seek(fi, buf[2], 1);		/* data */
	if(buf[7] != 1) {
		seek(fi, buf[1], 1);
		seek(fi, buf[2], 1);	/* reloc */
	}
	n = ldiv(0, buf[4], 12);
	if(n == 0) {
		printf(&quot;no name list\n&quot;);
		exit();
	}
	nlp = sbrk(12*n);
	read(fi, nlp, n*12);
	if (pflg==0)
		qsort(nlp, n, 12, compare);
	fout = dup(1);
	close(1);
	for(i=0; i&lt;n; i++) {
		if(gflg &amp;&amp; (nlp-&gt;typ&amp;040)==0)
			goto out;
		if(cflg) {
			if(nlp-&gt;name[0] != &#39;_&#39;)
				goto out;
			for(j=0; j&lt;7; j++)
				nlp-&gt;name[j] = nlp-&gt;name[j+1];
			nlp-&gt;name[7] = &#39;\0&#39;;
		}
		j = nlp-&gt;typ&amp;037;
		if(j &gt; 4)
			j = 1;
		if(j==0 &amp;&amp; nlp-&gt;val)
			j = 5;
		if(uflg &amp;&amp; j!=0)
			goto out;
		if(!uflg) {
			if(j==0)
				printf(&quot;      &quot;); else
				printo(nlp-&gt;val);
			printf(&quot;%c &quot;, (nlp-&gt;typ&amp;040? &quot;UATDBC&quot;:&quot;uatdbc&quot;)[j]);
		}
		printf(&quot;%.8s\n&quot;, nlp);
	out:
		nlp++;
	}
	flush();
}

compare(p1, p2)
struct nl *p1, *p2;
{
	int a, i;

	a = 0;
	if(nflg) {
		if(p1-&gt;val &gt; p2-&gt;val) {
			a = 1;
			goto out;
		}
		if(p1-&gt;val &lt; p2-&gt;val) {
			a = -1;
			goto out;
		}
	}
	for(i=0; i&lt;8; i++)
	if(p1-&gt;name[i] != p2-&gt;name[i]) {
		if(p1-&gt;name[i] &gt; p2-&gt;name[i])
			a = 1; else
			a = -1;
		goto out;
	}
out:
	return(a*rflg);
}

printo(v)
{
	int i;

	printf(&quot;%c&quot;, v&lt;0?&#39;1&#39;:&#39;0&#39;);
	for(i=0; i&lt;5; i++) {
		printf(&quot;%c&quot;, ((v&gt;&gt;12)&amp;7)+&#39;0&#39;);
		v =&lt;&lt;3;
	}
}
