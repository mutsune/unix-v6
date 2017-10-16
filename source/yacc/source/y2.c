# include &quot;dextern&quot;
# define IDENTIFIER 257
# define MARK 258
# define TERM 259
# define LEFT 260
# define BINARY 261
# define RIGHT 262
# define PREC 263
# define LCURLY 264
# define C_IDENTIFIER 265  /* name followed by colon */
# define NUMBER 266

setup(argc,argv) int argc; char *argv[];
{	int i,j,lev,t;
	int c;

	foutput = -2;
	i = 1;
	while( argc &gt;= 2  &amp;&amp; argv[1][0] == &#39;-&#39; ) {
		while( *++(argv[1]) ){
			switch( *argv[1] ){
			case &#39;v&#39;:
			case &#39;V&#39;:
				foutput = copen(&quot;y.output&quot;, &#39;w&#39; );
				if( foutput &lt; 0 ) error( &quot;cannot open y.output&quot;);
				continue;
			case &#39;o&#39;:
			case &#39;O&#39;:
				oflag = 1;
				continue;
			case &#39;r&#39;:
			case &#39;R&#39;:
				oflag = 1;
				rflag = 1;
				continue;
			default:  error( &quot;illegal option: %c&quot;, *argv[1]);
				}
			}
		argv++;
		argc--;
		}

	ftable = copen( oflag ? &quot;yacc.tmp&quot; : &quot;y.tab.c&quot; , &#39;w&#39; );
	if( ftable&lt;0 ) error( &quot;cannot open table file&quot; );
	if( argc &gt; 1 ) cin = copen( argv[1], &#39;r&#39; );
	if( cin &lt; 0 ) error( &quot;cannot open input&quot; );
	settab();
	printf(&quot;#\n&quot;);
	ctokn = &quot;$end&quot;;
	defin(0);  /* eof */
	extval = 0400;  /* beginning of assigned values */
	ctokn = &quot;error&quot;;
	defin(0);
	ctokn = &quot;$accept&quot;;
	defin(1);
	mem=mem0;
	cnamp = cnames;
	lev=0;
	i=0;

	while( t = gettok() ){
		switch( t ){
			case IDENTIFIER:	j = chfind(0);
					trmlev[j] = lev;
					continue;
			case &#39;,&#39;:
			case &#39;;&#39;:		continue;
			case TERM:		lev=0; continue;
			case LEFT:		lev=(++i&lt;&lt;3)|01; continue;
			case BINARY:	lev=(++i&lt;&lt;3)|02; continue;
			case RIGHT:	lev=(++i&lt;&lt;3)|03; continue;
			case MARK:
					defout();
					if( rflag ){ /* RATFOR */
						printf( &quot;define yyerrok yyerrf = 0\n&quot; );
						printf( &quot;define yyclearin yychar = -1\n&quot; );
						printf( &quot;subroutine yyactr(yyprdn)\n&quot;);
						printf( &quot;common/yycomn/yylval,yyval,yypv,yyvalv(150)\n&quot; );
						printf( &quot;common/yylcom/yychar,yyerrf,yydebu\n&quot; );
						printf( &quot;integer yychar, yyerrf, yydebu\n&quot; );
						printf( &quot;integer yyprdn,yyval,yylval,yypv,yyvalv\n&quot; );
						}
					else {
						printf( &quot;#define yyclearin yychar = -1\n&quot; );
						printf( &quot;#define yyerrok yyerrflag = 0\n&quot; );
						printf( &quot;extern int yychar, yyerrflag;\n&quot; );
						printf(&quot;\nint yyval 0;\nint *yypv;\nint yylval 0;&quot;);
						printf(&quot;\nyyactr(__np__){\n&quot;);
						}
					break;
			case LCURLY:	defout();
					cpycode();
					continue;
			case NUMBER:
				trmset[j].value = numbval;
				if( j &lt; ndefout &amp;&amp; j&gt;2 ) 
					error(&quot;please define type # of %s earlier&quot;, trmset[j].name );
				continue;
			default:	error(&quot;bad precedence syntax, input %d&quot;, t );
			}
		break;
		}
	prdptr[0]=mem;
	/* added production */
	*mem++ = NTBASE;
	*mem++ = NTBASE+1;
	*mem++ = 1;
	*mem++ = 0;
	prdptr[1]=mem;
	i=0;

	/* i is 0 when a rule can begin, 1 otherwise */

	for(;;) switch( t=gettok() ) {
	case C_IDENTIFIER:		if( mem == prdptr[1] ) {  /* first time */
						if( rflag ){
							printf( &quot;goto 1000\n&quot; );
							}
						else printf(&quot;\nswitch(__np__){\n&quot;);
						}
				if( i != 0 ) error( &quot;previous rule not terminated&quot; );
				*mem = chfind(1);
				if( *mem &lt; NTBASE )error( &quot;token illegal on lhs of grammar rule&quot; );
				i=1;
				++mem;
				continue;
	case IDENTIFIER:
			*mem=chfind(1);
			if(*mem &lt; NTBASE)levprd[nprod]=trmlev[*mem];
			mem++;
			if(i==0) error(&quot;missing :&quot;);
			continue;
	case &#39;=&#39;:		levprd[nprod] =| 04;
				if( i==0 ) error(&quot;semicolon preceeds action&quot;);
			printf( rflag?&quot;\n%d &quot;:&quot;\ncase %d:&quot;, nprod );
			cpyact();
			printf( rflag ? &quot; return&quot; : &quot; break;&quot; );
	case &#39;|&#39;:
	case &#39;;&#39;:		if(i){
				*mem++ = -nprod;
				prdptr[++nprod] = mem;
				levprd[nprod]=0;
				i=0;}
			if (t==&#39;|&#39;){i=1;*mem++ = *prdptr[nprod-1];}
			continue;
	case 0:		/* End Of File */
	case MARK:	if( i != 0 ) error( &quot;rule not terminated before %%%% or EOF&quot; );
			settab();
			finact();
			/* copy the programs which follow the rules */
			if( t == MARK ){
				while (c=getchar()) putchar(c);
				}
			return;
	case PREC:	
		if( i==0 ) error( &quot;%%prec must appear inside rule&quot; );
		if( gettok()!=IDENTIFIER)error(&quot;illegal %%prec syntax&quot; );
		j=chfind(2);
		if(j&gt;=NTBASE)error(&quot;nonterminal %s illegal after %%prec&quot;, nontrst[j-NTBASE].name);
		levprd[nprod]=trmlev[j];
		continue;
	case LCURLY:	
		if( i!=0 ) error( &quot;%%{ appears within a rule&quot; );
		cpycode();
		continue;
	default: error( &quot;syntax error, input %d&quot;, t  );
	}
}

finact(){
	/* finish action routine */
	register i;

	if( rflag ){

		printf( &quot;\n1000 goto(&quot; );
		for( i=1; i&lt;nprod; ++i ){
			printf( &quot;%d,&quot;, (levprd[i]&amp;04)==0?999:i );
			}
		printf( &quot;999),yyprdn\n&quot; );
		printf( &quot;999 return\nend\n&quot; );
		printf( &quot;define YYERRCODE %d\n&quot;, trmset[2].value );
		}
	else {
		printf( &quot;\n}\n}\n&quot; );
		printf( &quot;int yyerrval %d;\n&quot;, trmset[2].value );
		}
	}
defin(t) {
/*	define ctokn to be a terminal if t=0
	or a nonterminal if t=1		*/
	char *cp,*p;
	int c;


        if (t) {
          if( ++nnonter &gt;= ntlim ) error(&quot;too many nonterminals, limit %d&quot;,ntlim);
	  nontrst[nnonter].name = ctokn;
	  return( NTBASE + nnonter );
          }
        else {
          if( ++nterms &gt;= tlim ) error(&quot;too many terminals, limit %d&quot;,tlim );
          trmset[nterms].name = ctokn;
	if( ctokn[0]==&#39; &#39; &amp;&amp; ctokn[2]==&#39;\0&#39; ) /* single character literal */
		trmset[nterms].value = ctokn[1];
	else if ( ctokn[0]==&#39; &#39; &amp;&amp; ctokn[1]==&#39;\\&#39; ) { /* escape sequence */
		if( ctokn[3] == &#39;\0&#39; ){ /* single character escape sequence */
			switch ( ctokn[2] ){
				 /* character which is escaped */
			case &#39;n&#39;: trmset[nterms].value = &#39;\n&#39;; break;
			case &#39;r&#39;: trmset[nterms].value = &#39;\r&#39;; break;
			case &#39;b&#39;: trmset[nterms].value = &#39;\b&#39;; break;
			case &#39;t&#39;: trmset[nterms].value = &#39;\t&#39;; break;
			case &#39;\&#39;&#39;: trmset[nterms].value = &#39;\&#39;&#39;; break;
			case &#39;&quot;&#39;: trmset[nterms].value = &#39;&quot;&#39;; break;
			case &#39;\\&#39;: trmset[nterms].value = &#39;\\&#39;; break;
			default: error( &quot;invalid escape&quot; );
				}
			}
		else if( ctokn[2] &lt;= &#39;7&#39; &amp;&amp; ctokn[2]&gt;=&#39;0&#39; ){ /* \nnn sequence */
			if( ctokn[3]&lt;&#39;0&#39; || ctokn[3] &gt; &#39;7&#39; || ctokn[4]&lt;&#39;0&#39; ||
				ctokn[4]&gt;&#39;7&#39; || ctokn[5] != &#39;\0&#39; ) error(&quot;illegal \\nnn construction&quot; );
			trmset[nterms].value = 64*(ctokn[2]-&#39;0&#39;)+8*(ctokn[3]-&#39;0&#39;)+ctokn[4]-&#39;0&#39;;
			if( trmset[nterms].value == 0 ) error( &quot;&#39;\\000&#39; is illegal&quot; );
			}
		}
	else {
		trmset[nterms].value = extval++;

		}
	trmlev[nterms] = 0;
	return( nterms );
          }
}

defout(){ /* write out the defines (at the end of the declaration section) */

	_REGISTER int i, c;
	_REGISTER char *cp;

	for( i=ndefout; i&lt;=nterms; ++i ){

		cp = trmset[i].name;
		if( *cp == &#39; &#39; ) ++cp;  /* literals */

		for( ; (c= *cp)!=&#39;\0&#39;; ++cp ){

			if( c&gt;=&#39;a&#39; &amp;&amp; c&lt;=&#39;z&#39; ||
			    c&gt;=&#39;A&#39; &amp;&amp; c&lt;=&#39;Z&#39; ||
			    c&gt;=&#39;0&#39; &amp;&amp; c&lt;=&#39;9&#39; ||
			    c==&#39;_&#39; )  ; /* VOID */
			else goto nodef;
			}

		/* define it */

		printf( &quot;%c define %s %d\n&quot;, rflag?&#39; &#39;:&#39;#&#39;, trmset[i].name, trmset[i].value );

	nodef:	;
		}

	ndefout = nterms+1;

	}

chstash( c ){
  /* put character away into cnames */
  if( cnamp &gt;= &amp;cnames[cnamsz] ) error(&quot;too many characters in id&#39;s and literals&quot; );
  else *cnamp++ = c;
  }

int gettok() {
	int j, base;
	static int peekline; /* number of &#39;\n&#39; seen in lookahead */
	auto int c, match, reserve;

begin:
	reserve = 0;
        if( peekc&gt;=0 ) {
		c = peekc;
		lineno =+ peekline;
		peekc = -1;
		peekline = 0;
		}
        else c = getchar();
        while( c==&#39; &#39; || c==&#39;\n&#39; || c==&#39;\t&#39; ){
          if( c == &#39;\n&#39; ) ++lineno;
          c=getchar();
          }
	if (c==&#39;/&#39;)
		{if (getchar()!=&#39;*&#39;)error(&quot;illegal /&quot;);
		c=getchar();
		while(c) {
			if( c == &#39;\n&#39; ) ++lineno;
			if (c==&#39;*&#39;)
				{if((c=getchar())==&#39;/&#39;)break;}
			else c=getchar();}
		if (!c) return(0);
		goto begin;}
	j=0;
	switch(c){
	case &#39;&quot;&#39;:	
	case &#39;\&#39;&#39;:	match = c;
			ctokn = cnamp;
			chstash( &#39; &#39; );
			while(1){
				c = getchar();
				if( c == &#39;\n&#39; || c == &#39;\0&#39; )
					error(&quot;illegal or missing &#39; or \&quot;&quot;);
				if( c == &#39;\\&#39; ){
					c = getchar();
					chstash( &#39;\\&#39; );
					}
				else if( c == match ) break;
				chstash( c );
				}
			break;
	case &#39;%&#39;:
	case &#39;\\&#39;:	switch(c=getchar())
		{case &#39;0&#39;:	return(TERM);
		case &#39;&lt;&#39;:	return(LEFT);
		case &#39;2&#39;:	return(BINARY);
		case &#39;&gt;&#39;:	return(RIGHT);
		case &#39;%&#39;:
		case &#39;\\&#39;:	return(MARK);
		case &#39;=&#39;:	return(PREC);
		case &#39;{&#39;:	return(LCURLY);
		default:	reserve = 1;
		}
	default:	if( c &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39; ){ /* number */
				numbval = c-&#39;0&#39; ;
				base = (c==&#39;0&#39;) ? 8 : 10 ;
				for( c=getchar(); c&gt;=&#39;0&#39; &amp;&amp; c&lt;=&#39;9&#39;; c=getchar() ){
					numbval = numbval*base + c - &#39;0&#39;;
					}
				peekc = c;
				return(NUMBER);
				}
			else if( (c&gt;=&#39;a&#39;&amp;&amp;c&lt;=&#39;z&#39;)||(c&gt;=&#39;A&#39;&amp;&amp;c&lt;=&#39;Z&#39;)||c==&#39;_&#39;||c==&#39;.&#39;||c==&#39;$&#39;){
				ctokn = cnamp;
				while(	(c&gt;=&#39;a&#39;&amp;&amp;c&lt;=&#39;z&#39;) ||
					(c&gt;=&#39;A&#39;&amp;&amp;c&lt;=&#39;Z&#39;) ||
					(c&gt;=&#39;0&#39;&amp;&amp;c&lt;=&#39;9&#39;) ||
					c==&#39;_&#39; || c==&#39;.&#39; || c==&#39;$&#39; ) {
					chstash( c );
					if( peekc&gt;=0 ) { c = peekc; peekc = -1; }
					else c = getchar();
					}
				}
			else return(c);

			peekc=c;
			}
	chstash( &#39;\0&#39; );

	if( reserve ){ /* find a reserved word */
		if( compare(&quot;term&quot;)) return( TERM );
		if( compare(&quot;TERM&quot;)) return( TERM );
		if( compare(&quot;token&quot;)) return( TERM );
		if( compare(&quot;TOKEN&quot;)) return( TERM );
		if( compare(&quot;left&quot;)) return( LEFT );
		if( compare(&quot;LEFT&quot;)) return( LEFT );
		if( compare(&quot;nonassoc&quot;)) return( BINARY );
		if( compare(&quot;NONASSOC&quot;)) return( BINARY );
		if( compare(&quot;binary&quot;)) return( BINARY );
		if( compare(&quot;BINARY&quot;)) return( BINARY );
		if( compare(&quot;right&quot;)) return( RIGHT );
		if( compare(&quot;RIGHT&quot;)) return( RIGHT );
		if( compare(&quot;prec&quot;)) return( PREC );
		if( compare(&quot;PREC&quot;)) return( PREC );
		error(&quot;invalid escape, or illegal reserved word: %s&quot;, ctokn );
		}

	/* look ahead to distinguish IDENTIFIER from C_IDENTIFIER */

  look:
	while( peekc==&#39; &#39; || peekc==&#39;\t&#39; || peekc == &#39;\n&#39; ) {
		if( peekc == &#39;\n&#39; ) ++peekline;
		peekc = getchar();
		}

	if( peekc != &#39;:&#39; ) return( IDENTIFIER );
	peekc = -1;
	lineno =+ peekline;
	peekline = 0;
	return( C_IDENTIFIER );
}
chfind(t)

{	int i,j;

	if (ctokn[0]==&#39; &#39;)t=0;
	for(i=1;i&lt;=nterms;i++)
		if(compare(trmset[i].name)){
			cnamp = ctokn;
			return( i );
			}
	for(i=1;i&lt;=nnonter;i++)
		if(compare(nontrst[i].name)) {
			cnamp = ctokn;
			return( i+NTBASE );
			}
	/* cannot find name */
	if( t&gt;1 &amp;&amp; ctokn[0] != &#39; &#39; )
		error( &quot;%s should have been defined earlier&quot;, ctokn );
	return( defin( t ) );
	}

cpycode(){ /* copies code between \{ and \} */

	int c;
	c = getchar();
	if( c == &#39;\n&#39; ) {
		c = getchar();
		lineno++;
		}
	while( c ){
		if( c==&#39;\\&#39; )
			if( (c=getchar()) == &#39;}&#39; ) return;
			else putchar(&#39;\\&#39;);
		if( c==&#39;%&#39; )
			if( (c=getchar()) == &#39;}&#39; ) return;
			else putchar(&#39;%&#39;);
		putchar( c );
		if( c == &#39;\n&#39; ) ++lineno;
		c = getchar();
		}
	error(&quot;eof before %%}&quot;);
	}

cpyact(){ /* copy C action to the next ; or closing } */
	int brac, c, match, *i, j, s;

	brac = 0;

loop:
	c = getchar();
swt:
	switch( c ){

case &#39;;&#39;:
		if( brac == 0 ){
			putchar( c );
			return;
			}
		goto lcopy;

case &#39;{&#39;:
		brac++;
		goto lcopy;

case &#39;$&#39;:
		s = 1;
		c = getchar();
		if( c == &#39;$&#39; ){
			printf(&quot;yyval&quot;);
			goto loop;
			}
		if( c == &#39;-&#39; ){
			s = -s;
			c = getchar();
			}
		if( c&gt;=&#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39; ){
			j=0;
			while( c&gt;=&#39;0&#39; &amp;&amp; c&lt;= &#39;9&#39; ){
				j= j*10+c-&#39;0&#39;;
				c = getchar();
				}
			if( rflag ) printf( &quot;yyvalv(yypv%c%d)&quot;, s==1?&#39;+&#39;:&#39;-&#39;, j );
			else printf(&quot;yypv[%d]&quot;, s*j );
			goto swt;
			}
		putchar( &#39;$&#39; );
		if( s&lt;0 ) putchar(&#39;-&#39;);
		goto swt;

case &#39;}&#39;:
		brac--;
		if( brac == 0 ){
			putchar( c );
			return;
			}
		goto lcopy;

case &#39;/&#39;:	/* look for comments */
		putchar( c );
		c = getchar();
		if( c != &#39;*&#39; ) goto swt;

		/* it really is a comment */

		putchar( c );
		while( c=getchar() ){
			if( c==&#39;*&#39; ){
				putchar( c );
				if( (c=getchar()) == &#39;/&#39; ) goto lcopy;
				}
			putchar( c );
			}
		error( &quot;EOF inside comment&quot; );

case &#39;\&#39;&#39;:	/* character constant */
		match = &#39;\&#39;&#39;;
		goto string;

case &#39;&quot;&#39;:	/* character string */
		match = &#39;&quot;&#39;;

	string:

		putchar( c );
		while( c=getchar() ){

			if( c==&#39;\\&#39; ){
				putchar( c );
				c=getchar();
				}
			else if( c==match ) goto lcopy;
			putchar( c );
			}
		error( &quot;EOF in string or character constant&quot; );

case &#39;\0&#39;:
		error(&quot;action does not terminate&quot;);
case &#39;\n&#39;:	++lineno;
		goto lcopy;

		}

lcopy:
	putchar( c );
	goto loop;
	}
