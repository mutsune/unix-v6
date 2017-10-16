/*int	mbuf[1024];	/*INSTR*/
int	ig	100;
int	esw;
int	initf	1;
char	ssp[270];
char	*sym[9]	{ssp,
		ssp + 30,
		ssp + 60,
		ssp + 90,
		ssp + 120,
		ssp + 150,
		ssp + 180,
		ssp + 210,
		ssp + 240,
		ssp + 270
		};

main(argc, argv)
	char	*argv[];
{
	auto	f,fct,file,i;

	if(argc &lt; 2) {
		printf(&quot;Usage: crpost [-d] file1 file2 ...\n&quot;);
		exit();
	}

	if(*argv[1] == &#39;-&#39;) {
		if(argv[1][1] == &#39;E&#39;) {
			fct = 3;
			esw = 1;
			goto on;
		}
		fct = argv[1][1] - &#39;0&#39;;
		if((fct &lt; 1) || (fct &gt; 9)) {
			printf(&quot;-d: 0 &lt; d &lt; 10\n&quot;);
			exit();
		}
		if(argv[1][2] == &#39;x&#39;) {
			ig = fct-1;
		}
on:
		argv++;
		argc--;
	} else {
		fct = 3;
	}

	f = 0;
	while(++f &lt; argc) {
		file = open(argv[f], 0);

		edf(file, fct);

		close(file);
	}
	flsh(0);

	exit();
}

int	lno	1;

edf(file, fct)
{
	auto	i,j,l;
	extern	lno,etext;
	char	brk,*buf,bufsp[150],line[150];
	char extern	*sym[];

/*	monitor(&amp;main,&amp;etext,&amp;mbuf,1024);	/*INSTR*/
	for(i = 0; i &lt; fct; i++)
		*sym[i] = 0200;
fields:
	l = -1;
	buf = &amp;bufsp;

	for(i = 0; i &lt; fct; ++i) {
		buf--;

swt:

		switch(*++buf = get(file)) {

			default:
				if(esw &amp;&amp; *buf &gt;= &#39;A&#39; &amp;&amp; *buf &lt;= &#39;Z&#39;
					&amp;&amp; i == 0)
					*buf =| 040;
				goto swt;

			case -1:
				*buf = &#39; &#39;;
			case &#39;\t&#39;:
				if(i == ig)	continue;
				brk = *buf;
				*buf = &#39;\0&#39;;
				buf = &amp;bufsp;
				if(comp(buf, sym[i])) {
					if(esw &amp;&amp; i == 0) {
						line[0] = line[1] = &#39;\t&#39;;
						l = 1;
						goto rest;
					}
					line[++l] = &#39;\t&#39;;
					continue;
				} else {
					copy(buf, sym[i]);
					l =+ copy(buf, &amp;line[++l]);
					line[--l] = brk;
					if(l &lt; 8 &amp;&amp; esw &amp;&amp; i == 0)
						line[++l] = &#39;\t&#39;;
					j = i;
					while(++j &lt; fct)
						*sym[j] = 0200;
					continue;
				}

			case &#39;\n&#39;:
				lno++;
				brk = *buf;
				*buf = &#39;\0&#39;;
				buf = &amp;bufsp;
				if(comp(buf, sym[i])) {
					goto fields;
				} else {
					copy(buf, sym[i]);
					l =+ copy(buf, &amp;line[++l]);
					line[--l] = &#39;\n&#39;;
					j = i;
					while(++j &lt; fct)
						*sym[j] = 0;
					goto out;
				}

			case &#39;\0&#39;:
				goto fexit;
		}
	}

rest:
	while((line[++l] = get(file)) != &#39;\n&#39;)
		if(line[l] == &#39;\0&#39;)	goto fexit;

	lno++;
out:
	if(*line != &#39;\t&#39;) {
		put(0,&quot;\n&quot;,1);
		lno++;
	}

	put(0,line,++l);

	goto fields;

fexit:
/*	monitor(0);	/*INSTR*/
	return(0);

}


copy(a, b)
	char	*a,*b;
{
	char	*c;

	b--;
	c = --a;
	while(*++b = *++a);
	return(a - c);
}

comp(a, b)
	char	*a, *b;
{
/*	printf(&quot;comp: %s %s\n&quot;,a,b); /*DEBUG*/
	a--;
	b--;
	while(*++a == *++b) {
		if(*a == &#39;\0&#39;)	return(1);
	}
	return(0);
}


char	buf[512];
int	nread	1;

get(ifile) int ifile;
{

	char static *ibuf;

	if(--nread){
		return(*ibuf++);
	}

	if(nread = read(ifile,buf,512)){
		if(nread &lt; 0)goto err;

		ibuf = buf;
		return(*ibuf++);
	}

	nread = 1;
	return(0);

err:
	nread = 1;
	printf(&quot;read error\n&quot;);
	return(0);

}


int	tp[1]	1;
int	optr[4];
char	bsp[512];

char	*obuf[1]	bsp;

int	nflush;

put(fil,string,n)
	char	*string;
{
	int	i;
	char	*o;

/*printf(&quot;%d %c %d\n&quot;,fil,*string,n);/*DEBUG*/

	string--;

	if((i = optr[fil] + n - 512) &gt;= 0) {
		n =- i;
		o = &amp;obuf[fil][optr[fil]] -1;
		while(--n &gt;= 0)
			*++o = *++string;
		optr[fil] = 512;
		flsh(fil);
		n = i;
	}

	o = &amp;obuf[fil][optr[fil]] - 1;
	optr[fil] =+ n;

	while(--n &gt;= 0) {
		*++o = *++string;
	}
	return(0);
}

flsh(fil)
{
	extern	tp[];

	if(optr[fil] &lt;= 0)	return(optr[fil]);

	nflush++;
	if(write(tp[fil],obuf[fil],optr[fil]) != optr[fil])
		return(-1);
	optr[fil] = 0;
	return(0);
}

