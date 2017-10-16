/* Ratfor-Fortran command */

extern int fin, fout;
char	ts[1000];
char	*tsp	ts;
char	*av[50];
char	*rlist[50];
int	nr	0;
char	*llist[50];
int	nl	0;
int	nxo	0;
int	bdcount	0;	/* count block data files generated */
int	rflag;
int	dflag	0;
int	vflag	1;
int	fflag;
int	cflag;
char	*complr &quot;/usr/fort/fc1&quot;;
char	*ratfor &quot;/usr/lib/ratfor&quot;;

main(argc, argv)
char *argv[]; {
	char *t;
	int i, j, c;
	int dexit();

	for(i=0; ++i &lt; argc; ) {
		if(*argv[i] == &#39;-&#39;)
			switch (argv[i][1]) {
				default:
					goto passa;
				case &#39;d&#39;:
					dflag = 1;
					break;
				case &#39;v&#39;:
					vflag = 0;
					break;
				case &#39;r&#39;:
					rflag = fflag = cflag = 1;
					break;
				case &#39;f&#39;:
					fflag = 1;
					break;
				case &#39;c&#39;:
					cflag = 1;
					break;
				case &#39;2&#39;:
					complr = &quot;/usr/fort/fc2&quot;;
					break;
			}
		else {
	   passa:
			t = argv[i];
			if( (c=getsuf(t))==&#39;r&#39; )
				ratcomp(t);
			else if( c==&#39;f&#39;)  {
				fortcomp(t);
				llenter(setsuf(copy(t),&#39;o&#39;));
			}
			else
				llenter(copy(t));
		}
	}
	if(rflag)
		dexit();
	if ((signal(2, 1) &amp; 01) == 0)
		signal(2, &amp;dexit);
	if(dflag)
		printf(&quot;cflag=%d, nl=%d\n&quot;, cflag, nl);
	if (cflag==0 &amp;&amp; nl!=0) {
		i = 0;
		av[0] = &quot;ld&quot;;
		av[1] = &quot;-x&quot;;
		av[2] = &quot;/lib/fr0.o&quot;;
		j = 3;
		while(i&lt;nl)
			av[j++] = llist[i++];
		av[j++] = &quot;-lf&quot;;
		av[j++] = &quot;/lib/filib.a&quot;;
		av[j++] = &quot;-l&quot;;
		av[j++] = 0;
		callsys(&quot;/bin/ld&quot;, av);
	}
	dexit();
}

dexit()
{
	int i;
	cunlink(&quot;ratjunk&quot;);
	cunlink(&quot;f.tmp1&quot;);
	exit(0);
}


ratcomp(s) char *s; {
	int i,j,t,nerr,status;
	nr = 0;
	if(vflag)
		printf(&quot;%s:\n&quot;,s);
	av[0] = ratfor;
	av[1] = s;
	av[2] = 0;
	if( (t=fork())==0 ){
		close(1);
		fout = creat(&quot;ratjunk&quot;, 0666);
		execv(ratfor, av);
		fout = 2;
		error(&quot;can&#39;t ratfor\n&quot;);
		exit(1);
	}
	while( t!=wait(&amp;status) );
	if( (t=(status&amp;0377)) != 0 &amp;&amp; t!=14 )
		dexit(1);
	t = (status&gt;&gt;8) &amp; 0377;
	if( t )
		return(++cflag);
	splitup();
	nerr=0;
	for(i=0; i&lt;nr; i++){
		if( vflag ) printf(&quot;   &quot;);
		if( fortcomp(rlist[i]) )
			nerr++;
	}
	if( nerr )
		return(1);
	av[0] = &quot;ld&quot;;
	av[1] = &quot;-r&quot;;
	av[2] = &quot;-x&quot;;
	j = 3;
	for(i=0; i&lt;nr; i++)
		av[j++] = rlist[i];
	av[j] = 0;
	callsys(&quot;/bin/ld&quot;, av);
	t = setsuf(copy(s),&#39;o&#39;);
	if( move(&quot;a.out&quot;, t) )
		cflag++;
	llenter(t);
	for(i=0; i&lt;nr; i++) {
		if( nodup(llist,rlist[i]) )
			cunlink(rlist[i]);
		if( fflag==0 )
			cunlink(setsuf(rlist[i],&#39;f&#39;));
	}
}

fortcomp(s) char *s; {
	int t;
	if( vflag ) printf(&quot;%s:\n&quot;, s);
	av[0] = complr;
	av[1] = s;
	av[2] = 0;
	if( callsys(complr, av) )
		return(++cflag);
	av[0] = &quot;as&quot;;
	av[1] = &quot;-&quot;;
	av[2] = &quot;f.tmp1&quot;;
	av[3] = 0;
	callsys(&quot;/bin/as&quot;, av);
	t = setsuf(s, &#39;o&#39;);
	if( move(&quot;a.out&quot;, t) )
		return(++cflag);
	return(0);
}

getsuf(s)
char s[];
{
	int c;
	char t, *os;

	c = 0;
	os = s;
	while(t = *s++)
		if (t==&#39;/&#39;)
			c = 0;
		else
			c++;
	s =- 3;
	if (c&lt;=14 &amp;&amp; c&gt;2 &amp;&amp; *s++==&#39;.&#39;)
		return(*s);
	return(0);
}

setsuf(s, ch)
char s[];
{
	char *os;

	os = s;
	while(*s++);
	s[-2] = ch;
	return(os);
}

move(s,t) char *s, *t; {
	cunlink(t);
	if(link(s, t) || cunlink(s)) {
		printf(&quot;move failed: %s\n&quot;, t);
		return(1);
	}
	return(0);
}

callsys(f, v)
char f[], *v[]; {
	int i, t, status;

	if(dflag){
		for(i=0; v[i]; i++)
			printf(&quot;%s &quot;, v[i]);
		putchar(&#39;\n&#39;);
	}
	if ((t=fork())==0) {
		execv(f, v);
		printf(&quot;Can&#39;t find %s\n&quot;, f);
		exit(1);
	} else
		if (t == -1) {
			printf(&quot;Try again\n&quot;);
			return(1);
		}
	while(t!=wait(&amp;status));
	if ((t=(status&amp;0377)) != 0 &amp;&amp; t!=14) {
		if (t!=2)		/* interrupt */
			printf(&quot;Fatal error in %s\n&quot;, f);
		dexit();
	}
	t = (status&gt;&gt;8) &amp; 0377;
	if(dflag &amp;&amp; status != 0)
		printf(&quot;status = %d\n&quot;, t);
	return(t);
}

copy(s)
char s[]; {
	char *otsp;

	otsp = tsp;
	while(*tsp++ = *s++);
	return(otsp);
}

nodup(l, s)
char **l, s[]; {
	char *t, *os, c;

	if (getsuf(s) != &#39;o&#39;)
		return(1);
	os = s;
	while(t = *l++) {
		s = os;
		while(c = *s++)
			if (c != *t++)
				break;
		if (*t++ == &#39;\0&#39;)
			return(0);
	}
	return(1);
}

llenter(t) char *t; {
	if (nodup(llist, t)) {
		llist[nl++] = t;
		if (getsuf(t)==&#39;o&#39;)
			nxo++;
	}
}

cunlink(f)
char *f;
{
	if( dflag )
		printf(&quot;unlink %s\n&quot;, f);
	if (f==0)
		return(0);
	return(unlink(f));
}

splitup(){
	char in[200], fname[20];
	int buf[259];
	int i,fd,c;
	if( (fin=open(&quot;ratjunk&quot;, 0)) &lt; 0)
		error(&quot;can&#39;t open ratjunk\n&quot;);
	while( gets(in) ){
		getname(in, fname);
		savename(fname);
		if( (fd = fcreat(fname, buf)) &lt; 0)
			error(&quot;can&#39;t open %s&quot;, fname);
		puts(in,buf);
		while( ! endcard(in) ){
			gets(in);
			puts(in,buf);
		}
		fflush(buf);
		close(fd);
	}
	close(fin);
}

gets(s) char *s; {
	int c;
	while( (*s++=c=getchar()) != &#39;\n&#39; &amp;&amp; c != &#39;\0&#39; );
	*s = &#39;\0&#39;;
	return(c);
}

puts(s,b) char *s; int *b; {
	while( *s )
		putc(*s++, b);
}

savename(s) char *s; {
	rlist[nr++] = copy(s);
}

getname(s,f) char *s,*f; {
	int i,j,c;
   loop:
	while( *s == &#39; &#39; || *s == &#39;\t&#39; )
		s++;
	if( compar(s,&quot;subroutine&quot;) ){ s =+ 10; goto bot; }
	else if( compar( s,&quot;function&quot;) ){ s =+ 8; goto bot; }
	else if( compar(s,&quot;real&quot;) ){ s =+ 4; goto loop; }
	else if( compar(s,&quot;integer&quot;) ){ s =+ 7; goto loop; }
	else if( compar(s,&quot;logical&quot;) ){ s =+ 7; goto loop; }
	else if( compar(s,&quot;double&quot;) ){ s =+ 6; goto loop; }
	else if( compar(s,&quot;precision&quot;) ){ s =+ 9; goto loop; }
	else if( compar(s,&quot;complex&quot;) ){ s =+ 7; goto loop; }
	else if( compar(s,&quot;block&quot;) ){
		s = &quot;blockdata &quot;;
		s[9] = (bdcount++) + &#39;0&#39;;
		goto bot;
	}
	else {
		for(i=0; f[i]=&quot;MAIN.f&quot;[i]; i++);
		return;
	}
   bot:
	while( *s == &#39; &#39; || *s == &#39;\t&#39; )
		s++;
	for(i=0; alphanum(s[i]); i++)
		f[i] = s[i];
	f[i++] = &#39;.&#39;;
	f[i++] = &#39;f&#39;;
	f[i++] = &#39;\0&#39;;
}

compar(s,t) char *s,*t; {
	while( *t )
		if( *s++ != *t++ )
			return(0);
	return(1);
}

alphanum(c) int c; {
	return( (c&gt;=&#39;a&#39; &amp;&amp; c&lt;=&#39;z&#39;)
		|| (c&gt;=&#39;A&#39; &amp;&amp; c&lt;=&#39;Z&#39;)
		|| (c&gt;=&#39;0&#39; &amp;&amp; c&lt;=&#39;9&#39;) );
}

endcard(s) char *s; {
	if( *s==0 )
		return(1);
	while( *s==&#39; &#39; || *s==&#39;\t&#39; )
		s++;
	if( *s!=&#39;e&#39; || *(s+1)!=&#39;n&#39; || *(s+2)!=&#39;d&#39; || *(s+3)!=&#39;\n&#39; )
		return(0);
	return(1);
}

error(s1, s2){
	fout = 1;
	printf(s1,s2);
	putchar(&#39;\n&#39;);
	flush(1);
	cflag++;
}
