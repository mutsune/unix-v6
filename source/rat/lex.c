# include &quot;r.h&quot;

char *keyword []{
	&quot;do&quot;, &quot;DO&quot;,	/* have to be first */
	&quot;if&quot;, &quot;IF&quot;,
	&quot;else&quot;, &quot;ELSE&quot;,
	&quot;for&quot;, &quot;FOR&quot;,
	&quot;repeat&quot;, &quot;REPEAT&quot;,
	&quot;until&quot;, &quot;UNTIL&quot;,
	&quot;while&quot;, &quot;WHILE&quot;,
	&quot;break&quot;, &quot;BREAK&quot;,
	&quot;next&quot;, &quot;NEXT&quot;,
	&quot;define&quot;, &quot;DEFINE&quot;,
	&quot;include&quot;, &quot;INCLUDE&quot;,
	0};

#include &quot;y.tab.c&quot;

int keytran[]{
	0, 0,
	XIF, XIF,
	XELSE, XELSE,
	XFOR, XFOR,
	REPEAT, REPEAT,
	UNTIL, UNTIL,
	XWHILE, XWHILE,
	XBREAK, XBREAK,
	NEXT, NEXT,
	XDEFINE, XDEFINE,
	XINCLUDE, XINCLUDE,
	0};

int	svargc;
char	**svargv;
int	infile	0;
int	fd	0;
int	ninclude	0;
int	filestack[10];
int	linect[10];

main(argc,argv) int argc; char **argv; {
	contfld = errorflag = 0;
	if(argc&gt;1 &amp;&amp; argv[1][0]==&#39;-&#39;){
		if(argv[1][1]==&#39;6&#39;)
			contfld=6;
		argc--;
		argv++;
	}
	svargc = argc;
	svargv = argv;
	filestack[0] = infile = fd = ninclude = linect[0] = 0;
	if(--svargc&gt;0)
		if( (fd = filestack[0] = copen(svargv[++infile],&#39;r&#39;)) &lt; 0 ) {
			error(&quot;can&#39;t open %s&quot;, svargv[infile]);
			cexit(1);
		}
	yyparse();
	cexit(errorflag);
}

int	peek	-1;
int	nextchar	&#39;\n&#39;;

getc(){
	nextchar = (peek&lt;0) ? gchar(): peek;
	peek = -1;
	return(nextchar);
}

int	gcp	0;
char	gcbuf[300];
int	apos	-1;

gchar(){
	extern int linect[], nnames;
	extern char *names[], *nameptr[];
	int c,i,atype,t;
	if( c=gcbuf[gcp++] )
		return(c);
   loop:
	for(gcp=0; (c=gcbuf[gcp]=cgetc(fd))!=&#39;\0&#39; ; gcp++ ){
		if( gcbuf[0]== &#39;%&#39; ){
			while(putchar(cgetc(fd))!=&#39;\n&#39;);
			gcp = -1;
			++linect[ninclude];
			continue;
		}
		if( (atype=alphanum(c)) &amp;&amp; apos &lt; 0 ){
			apos = gcp;
			continue;
		}
		if( !atype )
			if( apos &gt;= 0 ){
				gcbuf[gcp] = &#39;\0&#39;;
				if( nnames&gt;0 &amp;&amp; (t=lookup(&amp;gcbuf[apos],names))&gt;=0){
					for(i=0;gcbuf[apos++]=nameptr[t][i];i++);
					gcp = apos-1;
				}
				apos = -1;
				gcbuf[gcp] = c;
			}
		if( c &lt; &#39; &#39; &amp;&amp; (c!=&#39;\n&#39; &amp;&amp; c!=&#39;\t&#39;) )	/* strip crap */
			c = gcbuf[gcp] = &#39; &#39;;
		if( c==&#39;#&#39; ){
			gcbuf[gcp] = &#39;\n&#39;;
			while( (c=cgetc(fd))!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;);
		}
		if( c==&#39;&quot;&#39; || c==&#39;\&#39;&#39; ){
			while( (gcbuf[++gcp]=t=cgetc(fd)) != c )
				if( t==&#39;\n&#39; ) {
					error(&quot;unbalanced quote&quot;);
					gcbuf[gcp] = c;
					gcbuf[++gcp] = c = &#39;\n&#39;;
					goto newline;
				}
			continue;
		}
	newline:
		if( c==&#39;\n&#39; ){
			gcbuf[gcp+1] = &#39;\0&#39;;
			gcp = 1;
			++linect[ninclude];
			return(gcbuf[0]);
		}
	}
	if(ninclude){
		cclose(filestack[ninclude--]);
		fd = filestack[ninclude];
		goto loop;
	}
	cclose(filestack[ninclude]);
	if(--svargc&gt;0){
		if( (fd = filestack[ninclude] = copen(svargv[++infile],&#39;r&#39;)) &lt; 0) {
			error(&quot;can&#39;t open %s&quot;, svargv[infile]);
			cexit(1);
		}
		linect[0] = 0;
		goto loop;
	}
	return(0);
}

inclstat(){
	int i,c;
	char fname[100];
	while( (c=getc())==&#39; &#39; || c==&#39;\t&#39; );
	peek = c;
	for(i=0; (fname[i]=c=getc())!=&#39;\n&#39; &amp;&amp; c!=&#39;;&#39; &amp;&amp; c!=&#39; &#39; &amp;&amp; c!=&#39;\t&#39;; i++);
	fname[i] = &#39;\0&#39;;
	if( (fd = copen(fname,&#39;r&#39;)) &lt; 0 ) {
		error(&quot;can&#39;t open %s&quot;, fname);
		cexit(1);
	}
	else {
		filestack[++ninclude] = fd;
		linect[ninclude] = 0;
	}
}

lookup(string,tbl) char *string; char *tbl[]; {
	register i,j, r;
	for(i=0; tbl[i]!=0; i++){ 
		for( j=0; (r=tbl[i][j])==string[j] &amp;&amp; r!=&#39;\0&#39;; j++);
		if( r == string[j] )
			return(i);
	}
	return( -1 );
}

char	str[200];
int	strp;
int	nstr;

yylex(){
	int c, type;
  top:
	while( (c=getc())==&#39; &#39; || c==&#39;\n&#39; || c==&#39;\t&#39; );
	yylval = c;
	switch(c){

	case &#39;\0&#39;:
		return(&#39;\0&#39;);
	case &#39;;&#39;:
		return(SCOL);
	case&#39;{&#39;:
		return(LCURL);
	case &#39;}&#39;:
		return(RCURL);
	}
	peek = c;
	nstr = getstr(str);
	yylval = &amp;str[0];
	if( alldigits(str) )
		return(DIGITS);
	type = lookup(str,keyword);
	if( keytran[type]==XDEFINE ) {
		defstat();
		goto top;
	} else if( keytran[type]==XINCLUDE ) {
		inclstat();
		goto top;
	} else if( type &gt; 1 )
		return(keytran[type]);
	else if( type &lt; 0 )
		return(XGOK);
	while( (c=getc())==&#39; &#39; || c==&#39;\t&#39; || c==&#39;\n&#39; );
	peek = c;
	if( c&gt;=&#39;a&#39; &amp;&amp; c&lt;=&#39;z&#39; || c&gt;=&#39;A&#39; &amp;&amp; c&lt;=&#39;Z&#39; )
		return(NEWDO);
	else
		return(OLDDO);
}

getstr(s) char *s; {
	int  c, sp;
	for (sp=0; (c=s[sp++]=getc())!=&#39; &#39; &amp;&amp; c!=&#39;\t&#39; &amp;&amp; c!=&#39;\n&#39; &amp;&amp; c!=&#39;{&#39; &amp;&amp; c!=&#39;}&#39;
		&amp;&amp; c!=&#39;;&#39; &amp;&amp; c!=&#39;(&#39; &amp;&amp; c!=&#39;)&#39; ; )
			if( c==&#39;\&#39;&#39; || c==&#39;&quot;&#39; )
				while( (s[sp++]=getc())!=c );
	peek = c;
	s[--sp]=&#39;\0&#39;;
	return(sp);
}

alldigits(s) char *s; {
	int c;
	if( *s == &#39;\0&#39; )
		return(0);
	while( (c = *s++) != &#39;\0&#39; )
		if( c&lt;&#39;0&#39; || c&gt;&#39;9&#39; )
			return(0);
	return(1);
}

int	dbg	0;

yyerror(){;}

alphanum(c) int c; {
	if(c&gt;=&#39;0&#39; &amp;&amp; c&lt;=&#39;9&#39;) return(1);
	if(c&gt;=&#39;a&#39; &amp;&amp; c&lt;=&#39;z&#39;) return(1);
	if(c&gt;=&#39;A&#39; &amp;&amp; c&lt;=&#39;Z&#39;) return(1);
	return(0);
}

#define	MAXNAMES	100

char	*names[MAXNAMES];
char	*nameptr[MAXNAMES];
int	nnames	0;

defstat(){
	int c,i,index;
	extern int peek,nstr;
	extern char str[];
	char *getvec();
	while( (c=getc())==&#39; &#39; || c==&#39;\t&#39; );
	peek = c;
	for(nstr=0; c=getc(); nstr++ ){
		if(c==&#39; &#39; || c==&#39;\t&#39; || c==&#39;\n&#39;) break;
		str[nstr] = c;
	}
	peek = c;
	str[nstr] = &#39;\0&#39;;
	if( (index=lookup(str,names)) &gt;= 0 )
		nameptr[index] = 0;
	else if( (index = nnames++)&gt;=MAXNAMES-1 ){
		error(&quot;too many defined names&quot;);
		cexit(1);
	}
	names[index] = getvec(nstr+1);
	for( i=0; names[index][i]=str[i]; i++ );
	while( (c=getc())==&#39; &#39; || c==&#39;\t&#39; );
	peek = c;
	for( i=0; (c=getc())!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;; i++ )
		str[i] = c;
	str[i] = &#39;\0&#39;;
	nameptr[index] = getvec(i+1);
	for( i=0; nameptr[index][i]=str[i]; i++ );
}

