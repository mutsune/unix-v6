#
# include &quot;econs.h&quot;
# include &quot;ecmn.h&quot;
/*int	mbuf[1024];	/*INSTR*/
/*int	tbuf[12];	/*INSTR*/

int	(*acts[])()	{0,
			&amp;coll,
			&amp;save,
			&amp;out,
			&amp;error,
			&amp;hyphen,
			&amp;pno
			};

char	*tmp[2]	{&quot;/tmp/crt0a&quot;,
		&quot;/tmp/crt1a&quot;
		};

char	*ignonl	&quot;/usr/lib/eign&quot;;
char	*gtab	&quot;/usr/lib/etab&quot;;

main(argc,argv)
	char	*argv[];
{
	auto	i,j,tm1,tm2,tm3;
	char	*fn,*av[8];
/*	extern	etext;	/*INSTR*/

/*	monitor(&amp;main,&amp;etext,&amp;mbuf,1024);	/*INSTR*/

	if(*argv[1] == &#39;-&#39;) {
		j = flags(argv);
		argv =+ j;
		argc =- j;
	}

	init();

	i = 0;
	if(argc == 1) {
		*ibuf1 = 0;
		curfl = 2;
		curf[0] = &#39;_&#39;;
		curf[1] = &#39;\t&#39;;
		goto pipe;
	}
	while(++i &lt; argc) {
		curs[4] = &#39;\t&#39;;

		if(fopen(argv[i],ibuf1) &lt; 0) {
			printf(&quot;Can&#39;t open %s\n&quot;,argv[i]);
			dexit();
		}

		curfl = 0;
		while((curf[curfl] = *argv[i]++) != 0 &amp;&amp; curfl &lt;=8)
			if(curf[curfl++] == &#39;/&#39;)	curfl = 0;
		curf[curfl++] = &#39;\t&#39;;
		if(curfl == 8) curf[8] = -1;

pipe:
		ibuf = ibuf1;
		lno = 1;

		driver();

		close(file);
	}
	flsh(0);
	close(tp[0]);


/*	monitor(0);	/*INSTR*/
/*	dexit();	/*INSTR*/

/*	times(tbuf);	/*INSTR*/
/*	tm1 = tbuf[0]/6;	/*INSTR*/
/*	tm2 = tbuf[1]/6;	/*INSTR*/
/*	printf(&quot;Prep:  %d  %d\n&quot;, tm1, tm2);	/*INSTR*/

/*	exit();	/*DEBUG*/
	fn = &quot;/bin/sort&quot;;
	av[0] = &quot;sort&quot;;
	av[1] = tmp[0];
	av[2] = &quot;-o&quot;;
	av[3] = tmp[0];
	av[4] = 0;

	callsys(fn, av);

/*	times(tbuf);	/*INSTR*/
/*	tm1 = tbuf[3]/6;	/*INSTR*/
/*	tm2 = tbuf[5]/6;	/*INSTR*/
/*	printf(&quot;Sort:  %d  %d\n&quot;, tm1, tm2);	/*INSTR*/

	if(usw) {
		fn = &quot;/usr/bin/upost&quot;;
		av[0] = &quot;upost&quot;;
		i = 0;
	} else if(count) {
		fn = &quot;count&quot;;
		av[0] = &quot;count&quot;;
		i = 0;
	} else {
		fn = &quot;/usr/bin/crpost&quot;;
		av[0] = &quot;crpost&quot;;
		av[1] = &quot;-E&quot;;
		i = 1;
	}
	av[++i] = tmp[0];
	av[++i] = 0;

	callsys(fn,av);

/*	times(tbuf);	/*INSTR*/
/*	tm1 = tbuf[3]/6 - tm1;	/*INSTR*/
/*	tm2 = tbuf[5]/6 - tm2;	/*INSTR*/
/*	printf(&quot;Post:  %d  %d\n&quot;, tm1, tm2);	/*INSTR*/

	dexit();
}

driver()
{
	auto	p;

top:
	l = -1;
	while((c = line[++l] = getc(ibuf)) != -1) {
/*	printf(&quot;driver: c = %o l = %d\n&quot;,c,l); /*DEBUG*/
		if(l &gt;= 299) {
			printf(&quot;Line too long: %d.\n&quot;,lno);
			dexit();
		}

		if(c &amp; 0200) {
			printf(&quot;Illegal character: %o line %d\n&quot;,c,lno);
			dexit();
		}

		if(fl) {
			if((*flag[fl])())
				continue;
		}

/*printf(&quot;cs = %d cc = %c ca = %d\n&quot;,cs,c,tab[cs].cl[c]);	/*DEBUG*/

		if(p = tab[cs].cl[c])
			(*acts[p])();
		continue;
	}
	if(ibuf == ibuf1)	return;
	ibuf = ibuf1;
	goto top;

}

init()
{
	int	b[3];
	auto	fi,i;
	extern	coll(),save(),out(),asym(),asw(),csym(),csw();
	extern	incl(),decl(),sk(),sk2();
	extern	dexit();

	ibuf1 = &amp;ib1;



	if((fi = open(gtab,0)) &lt; 0) {
		printf(&quot;Cannot open grammar table; see lem\n&quot;);
		dexit();
	}

	i = -1;
	while(++i &lt; NUMS)
		if(read(fi,tab[i].cl,256) &lt; 256) {
			printf(&quot;Bad grammar table; see lem\n&quot;);
			dexit();
		}

	close(fi);



	if(signal(1,1) != 1)	signal(1,&amp;dexit);
	if(signal(2,1) != 1)	signal(2,&amp;dexit);
	if(signal(3,1) != 1)	signal(3,&amp;dexit);
	while((tp[1] = creat(tmp[1],0)) &lt; 0)
		tmp[1][9]++;
	close(tp[1]);
	tmp[0][9] = tmp[1][9];
	tp[0] = creat(tmp[0],CREATC);

	if(count)	return;

	itab.hptr = &amp;ipsp;
	itab.symt = &amp;issp;
	itab.hsiz = PTRI;
	itab.ssiz = CHARI;
	itab.nsym = 0;
	itab.curb = 1;

	if((fi = open(ignonl,0)) &lt; 0) {
		printf(&quot;Cannot open ignore/only file.\n&quot;);
		dexit();
	}
	if((read(fi,b,6) == 6) &amp;&amp; (b[0] == 0100200)) {
		if(read(fi,itab.hptr,b[1]) &lt; b[1]) {
			printf(&quot;Cannot read ignore/only file.\n&quot;);
			dexit();
		}
		if(read(fi,itab.symt,b[2]) &lt; b[2]) {
			printf(&quot;Cannot read ignor/only file.\n&quot;);
			dexit();
		}
		close(fi);
	} else {
		close(fi);
		compile();
	}
	return;
}

error(a)
{
	printf(&quot;Error %d\n&quot;,a);
	dexit();
}

dexit()
{
	extern	nflush;

/*	printf(&quot;nflush = %d\n&quot;,nflush);	/*DEBUG*/
	if(tp[0] &gt; 0 &amp;&amp; utmp == 0) {
		unlink(tmp[0]);
		unlink(tmp[1]);
	}
	exit();
}

callsys(f,v)
	char	f[],*v[];
{
	int	t,status,i;

	if((t = fork()) == 0) {
		for(i = 1; i &lt;= 12; i++)	signal(i,0);
		execv(f,v);
		printf(&quot;Can&#39;t find %s\n&quot;,f);
		exit(1);
	} else {
		if(t == -1) {
			printf(&quot;Try again\n&quot;);
			return(1);
		}
	}

	while(t != wait(&amp;status));
/*	printf(&quot;Status = %o, %s\n&quot;,status,f);	/*DEBUG*/
	if((t = (status &amp; 0377)) != 0) {
		if(t != 2) {
			printf(&quot;Fatal error in %s\n&quot;,f);
			printf(&quot;t = %d\n&quot;,t);
		}
		dexit();
	}
	return((status&gt;&gt;8) &amp; 0377);
}

flags(argv)
	char	*argv[];
{
	int	j,xx;
	char	*ap;

	j = 1;
	ap = argv[1];
	while(*++ap != &#39;\0&#39;) {
		switch(*ap) {

			default:
				printf(&quot;Unrecognized flag: %c\n&quot;,*ap);
				dexit();

			case &#39;c&#39;:
				count = 1;
				continue;

			case &#39;i&#39;:	/* Ignore file */
				if(!xx) {
					xx = 1;
					only = 0;
					ignonl = argv[++j];
				}
				continue;

			case &#39;o&#39;:	/*only file*/
				if(!xx) {
					xx = 1;
					only = 1;
					ignonl = argv[++j];
				}
				continue;

			case &#39;p&#39;:
				page = 1;
				continue;

			case &#39;t&#39;:
				utmp = argv[++j];
				tmp[0] = argv[j];
				continue;

			case &#39;u&#39;:	/* Unique symbols only */
				usw = 1;
				continue;

			case &#39;w&#39;:	/* Word list only */
				word = 1;
				continue;

		}
	}
	return(j);
}


compile()
{
	char	buf[40],*b;
	int	i,v;

	fopen(ignonl,ibuf1);

	b = buf - 1;
	while((*++b = getc(ibuf1)) != -1) {
		if(*b == &#39;\n&#39;) {
			*b = &#39;\0&#39;;
			search(buf,b - buf,&amp;itab,1);
			b = buf - 1;
		} else {
			if(*b == &#39;\t&#39;) {
				v = 0;
				while((i = getc(ibuf1)) != -1) {
					if(i == &#39;\n&#39;)	break;
					v = v*10 + (i - &#39;0&#39;);
				}
				search(buf,b - buf,&amp;itab,v);
				b = buf - 1;
			} else {
				if((b - buf) &gt; 39) {
					printf(&quot;Ignore/only symbol too long.\n&quot;);
					dexit();
				}
			}
		}
	}
	close(ibuf1);
	return;
}
