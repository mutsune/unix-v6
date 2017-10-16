#
#include &quot;ne.h&quot;
#include &quot;y.tab.c&quot;

char *cntrl[] {
	&quot;sup&quot;, &quot;super&quot;, &quot;sub&quot;,
	&quot;.EN&quot;,
	&quot;sum&quot;, &quot;from&quot;, &quot;to&quot;,
	&quot;hat&quot;, &quot;dot&quot;, &quot;dotdot&quot;, &quot;bar&quot;, &quot;tilde&quot;, &quot;under&quot;,
	&quot;prod&quot;, &quot;int&quot;, &quot;integral&quot;, &quot;union&quot;, &quot;inter&quot;,
	&quot;pile&quot;, &quot;lpile&quot;, &quot;cpile&quot;, &quot;rpile&quot;, &quot;over&quot;, &quot;sqrt&quot;,
	&quot;above&quot;, &quot;size&quot;, &quot;font&quot;, &quot;roman&quot;, &quot;italic&quot;, &quot;bold&quot;,
	&quot;left&quot;, &quot;right&quot;,
	&quot;delim&quot;, &quot;DELIM&quot;,
	&quot;DEFINE&quot;,&quot;define&quot;,
	&quot;.gsize&quot;, &quot;.GSIZE&quot;, &quot;gsize&quot;, &quot;GSIZE&quot;, &quot;gfont&quot;, &quot;GFONT&quot;,
	&quot;HAT&quot;, &quot;DOT&quot;, &quot;DOTDOT&quot;, &quot;BAR&quot;, &quot;TILDE&quot;, &quot;UNDER&quot;,
	&quot;PROD&quot;, &quot;INT&quot;, &quot;INTEGRAL&quot;, &quot;UNION&quot;, &quot;INTER&quot;,
	&quot;SUM&quot;, &quot;FROM&quot;, &quot;TO&quot;,
	&quot;SUP&quot;, &quot;SUPER&quot;, &quot;SUB&quot;, &quot;PILE&quot;, &quot;LPILE&quot;, &quot;CPILE&quot;, &quot;RPILE&quot;, &quot;OVER&quot;, &quot;SQRT&quot;,
	&quot;ABOVE&quot;, &quot;SIZE&quot;, &quot;FONT&quot;, &quot;ROMAN&quot;, &quot;ITALIC&quot;, &quot;BOLD&quot;,
	&quot;LEFT&quot;, &quot;RIGHT&quot;, 
	&quot;up&quot;, &quot;UP&quot;, &quot;down&quot;, &quot;DOWN&quot;, &quot;fwd&quot;, &quot;FWD&quot;, &quot;back&quot;, &quot;BACK&quot;,
	&quot;mark&quot;, &quot;MARK&quot;, &quot;lineup&quot;, &quot;LINEUP&quot;,
	&quot;matrix&quot;, &quot;lcol&quot;, &quot;ccol&quot;, &quot;rcol&quot;, &quot;col&quot;,
	0};


int icntrl[]{
	SUPER, SUPER, SUB,
	0, /* EOF */
	SUM, FROM, TO,
	HAT, DOT, DOTDOT, BAR, TILDE, UNDER,
	PROD, INT, INT, UNION, INTER,
	PILE, LPILE, CPILE, RPILE, OVER, SQRT,
	ABOVE, SIZE, FONT, ROMAN, ITALIC, BOLD,
	LEFT, RIGHT,
	DELIM, DELIM,
	DEFINE, DEFINE,
	GSIZE, GSIZE, GSIZE, GSIZE, GFONT, GFONT,
	HAT, DOT, DOTDOT, BAR, TILDE, UNDER,
	PROD, INT, INT, UNION, INTER,
	SUM, FROM, TO,
	SUPER, SUPER, SUB, PILE, LPILE, CPILE, RPILE, OVER, SQRT,
	ABOVE, SIZE, FONT, ROMAN, ITALIC, BOLD,
	LEFT, RIGHT,
	UP, UP, DOWN, DOWN, FWD, FWD, BACK, BACK,
	MARK, MARK, LINEUP, LINEUP,
	MATRIX, LCOL, CCOL, RCOL, COL,
	0};

int	peek	-1;
#define	SSIZE	400
char	token[SSIZE];
int	sp;
int speek[10];
char *swt[10];
int sw -1;

getc(){
  loop:
	if(sw &gt;= 0){
		lastchar = (peek&lt;0) ? *swt[sw]++ : peek;
		peek = -1;
		if(lastchar != &#39;\0&#39;)return(lastchar);
		peek = speek[sw--];
		return(&#39; &#39;);
		}
	lastchar = (peek&lt;0) ? getchar() : peek;
	if( lastchar==&#39;\n&#39; )
		linect++;
	peek = -1;
	if( lastchar!= &#39;\0&#39; )
		return(lastchar);
	if( ++ifile &gt; svargc ){
		peek = &#39;\0&#39;;
		return(&#39;\0&#39;);
	}
	close(fin);
	linect = 1;
	if( (fin=open(svargv[ifile],0)) &gt;= 0 )
		goto loop;
	error(FATAL,&quot;can&#39;t open file %s\n&quot;, svargv[ifile]);
}

yylex(){
	int c, type;
  beg:
	while( (c=getc())==&#39; &#39; || c==&#39;\n&#39;);
	yylval=c;
	switch(c){

	case &#39;\0&#39;:
		return(&#39;\0&#39;);
	case &#39;~&#39;:
		return(SPACE);
	case &#39;^&#39;:
		return(THIN);
	case &#39;\t&#39;:
		return(TAB);
	case &#39;{&#39;:
		return(MQ);
	case &#39;}&#39;:
		return(MQ1);
	case &#39;&quot;&#39;:
		for(sp=0; (c=getc())!=&#39;&quot;&#39;; ){
			if(c !=&#39;\\&#39;)token[sp++]=c;
			else { if((c=getc())!= &#39;&quot;&#39;)token[sp++]=&#39;\\&#39;;
				token[sp++] = c; }
			if( sp&gt;=SSIZE )
				error(FATAL,&quot;quoted string %.20s... too long&quot;, token);
		}
		token[sp]=&#39;\0&#39;;
		yylval= &amp;token[0];
		return(QTEXT);
	}
	if( c==righteq )
		return(&#39;\0&#39;);

	getstr(token, c);
	if((type = lookup(token,nptr)) &gt;= 0){
		if(sw &gt;= 9)
			error(FATAL,&quot;definitions nested &gt; 9&quot;, sw);
		swt[++sw] = sptr[type];
		speek[sw] = peek;
		peek = -1;
		goto beg;
		}
	type = lookup(token,cntrl);
	if( type &lt; 0 )
		return(CONTIG);
	if( icntrl[type]==DEFINE ) {
		define();
		goto beg;
	}
	else if( icntrl[type]==DELIM ) {
		delim();
		goto beg;
	}
	else if( icntrl[type]==GSIZE ){
		globsize();
		goto beg;
	}
	else if( icntrl[type]==GFONT ) {
		globfont();
		goto beg;
	}
	else
		return( icntrl[type] );
}

getstr(s,c) char *s, c; {
	for (sp=0; c!=&#39; &#39; &amp;&amp; c!=&#39;\t&#39; &amp;&amp; c!=&#39;\n&#39; &amp;&amp; c!=&#39;{&#39; &amp;&amp; c!=&#39;}&#39;
		&amp;&amp; c!=&#39;&quot;&#39; &amp;&amp; c!=&#39;~&#39; &amp;&amp; c!=&#39;^&#39; &amp;&amp; c!=righteq; ) {
		if(c == &#39;\\&#39;) if((c = getc()) != &#39;&quot;&#39;)s[sp++] = &#39;\\&#39;;
		s[sp++] = c;
		if( sp&gt;=SSIZE )
			error(FATAL,&quot;token %.20s... too long&quot;,s);
		c = getc();
		}
	if( c==&#39;{&#39; || c==&#39;}&#39; || c==&#39;&quot;&#39; || c==&#39;~&#39; || c==&#39;^&#39; || c==&#39;\t&#39; || c==righteq )
		peek = c;
	s[sp]=&#39;\0&#39;;
	yylval = s;
}

lookup(str,tbl) char *str; char *tbl[]; {
	register i,j, r;
	for(i=0; tbl[i]!=0; i++){ /* table of tbl wds */
		for( j=0; (r=tbl[i][j])==str[j] &amp;&amp; r!=&#39;\0&#39;; j++);
		if( r == str[j] )
			return(i);
	}
	return( -1 );
}

cstr(s,quote) char *s; int quote; {
	int del,c,i;
	while((del=getc()) == &#39; &#39; || del == &#39;\t&#39; || del == &#39;\n&#39;);
	if(quote)
		for(i=0; (c=getc()) != del;)
			s[i++] = c;
	else {
		s[0] = del;
		for(i=1; (c=getc())!=&#39; &#39; &amp;&amp; c!= &#39;\t&#39; &amp;&amp; c!=&#39;\n&#39;;)
			s[i++]=c;
	}
	s[i] = &#39;\0&#39;;
	return(s);
}

define() {
	char *alloc();
	int i, c;
	while( (c=getc())==&#39; &#39; || c==&#39;\n&#39; );
	getstr(token,c);
	if((i = lookup(token,nptr)) &gt;= 0){
		yyval = i;
		free(sptr[yyval]);
	} else {
		yyval = ptr++;
		for(i=0; token[i] != &#39;\0&#39;; i++);
		nptr[yyval] = alloc(i+1);
		for(i=0; nptr[yyval][i]=token[i]; i++);
	}
	if(dbg)printf(&quot;.\tdefine %s\n&quot;,nptr[yyval]);
	cstr(token,1);
	for(i=0; token[i] != &#39;\0&#39;; i++);
	sptr[yyval] = alloc(i+1);
	for(i=0; sptr[yyval][i] = token[i]; i++);
	if(dbg)printf(&quot;.\tname %s defined as %s\n&quot;,nptr[yyval],sptr[yyval]);
}

delim() {
	char *s;
	yyval = eqnreg = 0;
	cstr(token,0);
	lefteq = token[0];
	righteq = token[1];
	if( (lefteq == &#39;o&#39; &amp;&amp; righteq == &#39;f&#39;) || (lefteq == &#39;O&#39; &amp;&amp; righteq == &#39;F&#39;) )
		lefteq = righteq = &#39;\0&#39;;
}

globsize() {
	extern int gsize;
	int c;
	while( (c=getc())==&#39; &#39; || c==&#39;\n&#39; );
	getstr(token,c);
	gsize = numb(token);
	yyval = eqnreg = 0;
}

globfont() {
	extern int gfont;
	while( (gfont=getc())==&#39; &#39; || gfont==&#39;\n&#39; );
	yyval = eqnreg = 0;
}
