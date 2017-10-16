/*int	mbuf[1024];	/*INSTR*/
int	psw	1;
int	initf	1;

main(argc, argv)
	char	*argv[];
{
	auto	f,fct,file;

	if(argc &lt; 2) {
		printf(&quot;Usage: cpost [-d] file1 file2 ...\n&quot;);
		exit();
	}

	if(*argv[1] == &#39;-&#39;) {
		fct = argv[1][1] - &#39;0&#39;;
		if((fct &lt; 1) || (fct &gt; 9)) {
			printf(&quot;-d: 0 &lt; d &lt; 10\n&quot;);
			exit();
		}
		argv++;
		argc--;
	} else {
		fct = 1;
	}

	f = 0;
	while(++f &lt; argc) {
		file = open(argv[f], 0);

		edl(file, fct);

		close(file);
		psw = 0;
	}
	flsh(0);

	exit();
}

int	lno	1;

edl(file)
{
	auto t,l;
	char	static	buf[20],fld[20],line[200];
	char	c;
field:
	t = -1;
	while(((buf[++t] = get(file)) != &#39;\t&#39;) &amp;&amp; (buf[t] != -1))
		if(buf[t] == &#39;\0&#39;)	goto done;

	if((c = buf[t]) == -1)	c = &#39; &#39;;
	buf[t] = &#39;\0&#39;;
	if(comp(buf,fld)) {
		lno++;
		goto junk;
	} else {
		if(lno == 0) {
			put(0,line,++l);
		}
		l = copy(buf,line);
		copy(buf,fld);
		line[--l] = c;
		lno = 0;
		goto fill;
	}

fill:
	while((line[++l] = get(file)) != &#39;\n&#39;) {
		if(line[l] == -1)	line[l] = &#39; &#39;;
		if(line[l] == &#39;\0&#39;)	goto done;
	}

	goto field;

junk:
	while((*line = get(file)) != &#39;\n&#39;)
		if(*line == &#39;\0&#39;)	goto done;

	goto field;

done:
	if(lno == 0)
		put(0,line,++l);
		lno = 1;
	return(0);

}



gfld(file, buf)
	char	*buf;
{
	char	c;

	buf--;
	while(*++buf  = get(file)) {
		if((*buf == &#39;\t&#39;) || (*buf == &#39;\n&#39;)) {
			c = *buf;
			*buf = &#39;\0&#39;;
			return(c);
		} else {
			continue;
		}
	}
	return(&#39;\0&#39;);
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

