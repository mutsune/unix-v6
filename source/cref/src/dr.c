#
# include &quot;../mcons.h&quot;
# include &quot;../ccmn.h&quot;
/*int	mbuf[1024];	/*INSTR*/
/*int	tbuf[36];	/*INSTR*/

int	(*acts[])()	{0,
			&amp;coll,
			&amp;save,
			&amp;out,
			&amp;asym,
			&amp;asw,
			&amp;csym,
			&amp;csw,
			&amp;incl,
			&amp;decl,
			&amp;sk2,
			&amp;sk,
			&amp;tabs,
			&amp;semi
			};

char	*tmp[5]	{&quot;/tmp/crt0a&quot;,
		&quot;/tmp/crt1a&quot;,
		&quot;/tmp/crt2a&quot;,
		&quot;/tmp/crt3a&quot;,
		&quot;/tmp/crt4a&quot;
		};

char	*ignonl	&quot;/usr/lib/aign&quot;;
char	*gtab	&quot;/usr/lib/atab&quot;;

main(argc,argv)
	char	*argv[];
{
	auto	i,j,tm1,tm2,tm3;
	char	*fn,*av[8];
/*	extern	etext;	/*INSTR*/

/*	monitor(&amp;main,&amp;etext,&amp;mbuf,1024);	/*INSTR*/
	if(argc &lt; 2) {
		printf(&quot;Usage: cref [-aceilosux] file1 ...\n&quot;);
		exit();
	}

	lbuf[4] = &#39;\t&#39;;
	if(*argv[1] == &#39;-&#39;) {
		j = flags(argv);
		argv =+ j;
		argc =- j;
	}

	init();

	i = 0;
	while(++i &lt; argc) {
		curs[0] = &#39;_&#39;;
		curs[1] = &#39;\t&#39;;
		curs[4] = &#39;\t&#39;;
		cursl = 2;

		if(fopen(argv[i],ibuf1) &lt; 0) {
			printf(&quot;Can&#39;t open %s\n&quot;,argv[i]);
			dexit();
		}
		ibuf = ibuf1;

		curf[0] = &#39;\t&#39;;
		curfl = 1;
		while((curf[curfl] = *argv[i]++) != 0 &amp;&amp; curfl &lt;= 8)
			if(curf[curfl++] == &#39;/&#39;)	curfl = 1;

		curf[curfl++] = &#39;\t&#39;;
		if(curfl == 10)	curf[9] = -1;
/*		printf(&quot;%s %d\n&quot;,curf,curfl);/*DEBUG*/

		lno = 1;

		driver();

		close(file);
	}
	for(j = 0; j &lt; 4;) {
		flsh(j,0);
		close(tp[j++]);
	}

/*	monitor(0);	/*INSTR*/
/*	dexit();	/*INSTR*/

/*	times(tbuf);	/*INSTR*/
/*	tm1 = tbuf[15]/6;	/*INSTR*/
/*	tm2 = tbuf[19]/6;	/*INSTR*/
/*	tm3 = tbuf[23]/6;	/*INSTR*/
/*	printf(&quot;Prep: %d  %d  %d\n&quot;,tm1,tm2,tm3);	/*INSTR*/

	if(utmp)	exit();
	fn = &quot;/bin/sort&quot;;
	av[0] = &quot;sort&quot;;
	av[1] = &quot;-a&quot;;
	av[3] = &quot;-o&quot;;
	av[5] = 0;
	for(i = 0; i &lt; 4; i++) {
		av[4] = av[2] = tmp[i];
		callsys(fn,av);
		if(utmp)	break;
	}

/*	times(tbuf);	/*INSTR*/
/*	tm1 = tbuf[27]/6;	/*INSTR*/
/*	tm2 = tbuf[31]/6;	/*INSTR*/
/*	tm3 = tbuf[35]/6;	/*INSTR*/
/*	printf(&quot;Sort: %d  %d  %d\n&quot;,tm1,tm2,tm3);	/*INSTR*/

	if(usw) {
		fn = &quot;/usr/bin/upost&quot;;
		av[0] = &quot;upost&quot;;
		i = 0;
	} else {
		fn = &quot;/usr/bin/crpost&quot;;
		av[0] = &quot;crpost&quot;;
		av[1] = cross? &quot;-4x&quot;: &quot;-3&quot;;
		i = 1;
	}
	j = -1;
	while(++j &lt; 4) {
		av[++i] = tmp[j];
		if(utmp)	break;
	}
	av[++i] = 0;

	callsys(fn,av);

/*	times(tbuf);	/*INSTR*/
/*	tm1 = tbuf[27]/6 - tm1;	/*INSTR*/
/*	tm2 = tbuf[31]/6 - tm2;	/*INSTR*/
/*	tm3 = tbuf[35]/6 - tm3;	/*INSTR*/
/*	printf(&quot;Post: %d  %d  %d\n&quot;,tm1,tm2,tm3);	/*INSTR*/

	dexit();
}

driver()
{
	auto	p;

top:
	l = -1;
	while((c = line[++l] = getc(ibuf)) != -1) {
		if(l &gt;= 131) {
			printf(&quot;Line too long: %d %s\n&quot;,lno,curf);
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
	ibuf2 = &amp;ib2;

	xtab.hptr = &amp;xpsp;
	xtab.symt = &amp;xssp;
	xtab.hsiz = PTRX;
	xtab.ssiz = CHARX;
	xtab.nsym = 0;
	xtab.curb = 1;

	itab.hptr = &amp;ipsp;
	itab.symt = &amp;issp;
	itab.hsiz = PTRI;
	itab.ssiz = CHARI;
	itab.nsym = 0;
	itab.curb = 1;

	if((fi = open(gtab,0)) &lt; 0) {
		printf(&quot;Cannot open grammar table; see lem\n&quot;);
		dexit();
	}

	i = -1;
	while(++i &lt; NUMS)
		if(read(fi,tab[i].cl,256) &lt; 256) {
			printf(&quot;Bad grammar table.\n&quot;);
			dexit();
		}

	close(fi);


	if((fi = open(ignonl,0)) &lt; 0) {
		printf(&quot;Cannot open ignore/only file: %s\n&quot;,ignonl);
		dexit();
	}
	if((read(fi,b,6) == 6) &amp;&amp; (b[0] == 0100200)) {
		if(read(fi,itab.hptr,b[1]) &lt; b[1]) {
			printf(&quot;Cannot read ignore/only file: %s\n&quot;,ignonl);
			dexit();
		}
		if(read(fi,itab.symt,b[2]) &lt; b[2]) {
			printf(&quot;Cannot read ignore/only file: %s\n&quot;,ignonl);
			dexit();
		}
		close(fi);
	} else {
		close(fi);
		compile();
	}

	if((signal(1,1) &amp; 1) == 0)	signal(1,&amp;dexit);
	if((signal(2,1) &amp; 1) == 0)	signal(2,&amp;dexit);
	if((signal(3,1) &amp; 1) == 0)	signal(3,&amp;dexit);
/*	signal(4,&amp;dexit);
	signal(5,&amp;dexit);
	signal(6,&amp;dexit);
	signal(7,&amp;dexit);
	signal(8,&amp;dexit);
	signal(10,&amp;dexit);
	signal(11,&amp;dexit);
	signal(12,&amp;dexit);
*/
	if(utmp == 0) {
		while((tp[4] = creat(tmp[4],0)) &lt; 0)
			tmp[4][9]++;
		close(tp[4]);
		tmp[0][9] = tmp[4][9];
		tmp[1][9] = tmp[4][9];
		tmp[2][9] = tmp[4][9];
		tmp[3][9] = tmp[4][9];
		tp[0] = creat(tmp[0],CREATC);
		tp[1] = creat(tmp[1],CREATC);
		tp[2] = creat(tmp[2],CREATC);
		tp[3] = creat(tmp[3],CREATC);
	} else {
		if((tp[0] = creat(utmp,CREATC)) &lt; 0) {
		printf(&quot;Can&#39;t create user&#39;s temp file.\n&quot;);
			exit();
		}
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
		unlink(tmp[2]);
		unlink(tmp[3]);
		unlink(tmp[4]);
	}
	exit();
}

callsys(f,v)
	char	f[],*v[];
{
	int	t,status,i;

	if((t = fork()) == 0) {
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

			case &#39;1&#39;:	/* Symbol first */
				order = 1;
				continue;

			case &#39;2&#39;:	/* Current file first */
				order = 2;
				continue;

			case &#39;3&#39;:	/* Current symbol first */
				order = 3;
				continue;

			case &#39;a&#39;:	/* Assembler */
				cflag = 0;
				continue;

			case &#39;c&#39;:	/* C */
				gtab = &quot;/usr/lib/ctab&quot;;
				if(!xx)
					ignonl = &quot;/usr/lib/cign&quot;;
				cflag = 1;
				continue;

			case &#39;e&#39;:	/* English */
				gtab = &quot;/usr/lib/etab&quot;;
				if(!xx)
					ignonl = &quot;/usr/lib/eign&quot;;
				continue;

			case &#39;i&#39;:	/* Ignore file */
				if(!xx) {
					xx = 1;
					only = 0;
					ignonl = argv[++j];
				}
				continue;

			case &#39;l&#39;:	/* Line numbers in col. 3 */
				cross = 0;
				continue;

			case &#39;o&#39;:	/* Only file */
				if(!xx) {
					xx = 1;
					only = 1;
					ignonl = argv[++j];
				}
				continue;

			case &#39;s&#39;:	/* Symbols in col. 3 */
				cross = 1;
				continue;

			case &#39;t&#39;:
				utmp = argv[++j];
				tmp[0] = argv[j];
				continue;

			case &#39;u&#39;:	/* Unique symbols only */
				usw = 1;
				continue;

			case &#39;x&#39;:	/* C externals */
				xsw = 1;
				gtab = &quot;/usr/lib/ctab&quot;;
				if(!xx)
					ignonl = &quot;/usr/lib/cign&quot;;
				cflag = 1;
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
					printf(&quot;Ignore/only symbol too long&quot;);
					dexit();
				}
			}
		}
	}
	close(ibuf1);
	return;
}
