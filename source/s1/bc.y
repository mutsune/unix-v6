%right &#39;=&#39;
%left &#39;+&#39; &#39;-&#39;
%left &#39;*&#39; &#39;/&#39; &#39;%&#39;
%right &#39;^&#39;
%left UMINUS

%term LETTER DIGIT SQRT _IF  FFF EQ
%term _WHILE _FOR NE LE GE INCR DECR
%term _RETURN _BREAK _DEFINE BASE OBASE SCALE
%term EQPL EQMI EQMUL EQDIV EQREM EQEXP
%term _AUTO DOT
%term QSTR

%{
char cary[1000], *cp { cary };
char string[1000], *str {string};
int crs &#39;0&#39;;
int rcrs &#39;0&#39;;  /* reset crs */
int bindx 0;
int lev 0;
int bstack[10] { 0 };
char *numb[15] {
  &quot; 0&quot;, &quot; 1&quot;, &quot; 2&quot;, &quot; 3&quot;, &quot; 4&quot;, &quot; 5&quot;,
  &quot; 6&quot;, &quot; 7&quot;, &quot; 8&quot;, &quot; 9&quot;, &quot; 10&quot;, &quot; 11&quot;,
  &quot; 12&quot;, &quot; 13&quot;, &quot; 14&quot; };
int *pre, *post;
%}
%%
start	: 
	|  start stat tail
		= output( $2 );
	|  start def dargs &#39;)&#39; &#39;{&#39; dlist slist &#39;}&#39;
		={	bundle( pre, $7, post );
			conout( $$, $2 );
			rcrs = crs;
			output( &quot;&quot; );
			lev = bindx = 0;
			}
	;

dlist	:  tail
	| dlist _AUTO dlets tail
	;

stat	:  e 
		={ bundle( $1, &quot;ps.&quot; ); }
	| 
		={ bundle( &quot;&quot; ); }
	|  QSTR
		={ bundle(&quot;[&quot;,$1,&quot;]P&quot;);}
	|  LETTER &#39;=&#39; e
		={ bundle( $3, &quot;s&quot;, $1 ); }
	|  LETTER &#39;[&#39; e &#39;]&#39; &#39;=&#39; e
		={ bundle( $6, $3, &quot;:&quot;, geta($1)); }
	|  LETTER EQOP e
		={ bundle( &quot;l&quot;, $1, $3, $2, &quot;s&quot;, $1 ); }
	|  LETTER &#39;[&#39; e &#39;]&#39; EQOP e
		={ bundle($3, &quot;;&quot;, geta($1), $6, $5, $3, &quot;:&quot;, geta($1));}
	|  _BREAK
		={ bundle( numb[lev-bstack[bindx-1]], &quot;Q&quot; ); }
	|  _RETURN &#39;(&#39; e &#39;)&#39;
		= bundle( $3, post, numb[lev], &quot;Q&quot; );
	|  _RETURN &#39;(&#39; &#39;)&#39;
		= bundle( &quot;0&quot;, post, numb[lev], &quot;Q&quot; );
	| SCALE e
		= bundle( $2, &quot;k&quot; );
	| SCALE &#39;=&#39; e
		= bundle( $3, &quot;k&quot;);
	| SCALE EQOP e
		= bundle(&quot;K&quot;,$3,$2,&quot;k&quot;);
	| BASE e
		= bundle( $2, &quot;i&quot; );
	| BASE &#39;=&#39; e
		= bundle($3, &quot;i&quot;);
	| BASE EQOP e
		= bundle(&quot;I&quot;,$3,$2,&quot;i&quot;);
	| OBASE e
		= bundle( $2, &quot;o&quot; );
	| OBASE &#39;=&#39; e
		= bundle($3,&quot;o&quot;);
	| OBASE EQOP e
		= bundle(&quot;O&quot;,$3,$2,&quot;o&quot;);
	|  &#39;{&#39; slist &#39;}&#39;
		={ $$ = $2; }
	|  FFF
		={ bundle(&quot;f&quot;); }
	|  error
		={ bundle(&quot;c&quot;); }
	|  _IF CRS BLEV &#39;(&#39; re &#39;)&#39; stat
		={	conout( $7, $2 );
			bundle( $5, $2, &quot; &quot; );
			}
	|  _WHILE CRS &#39;(&#39; re &#39;)&#39; stat BLEV
		={	bundle( $6, $4, $2 );
			conout( $$, $2 );
			bundle( $4, $2, &quot; &quot; );
			}
	|  fprefix CRS re &#39;;&#39; e &#39;)&#39; stat BLEV
		={	bundle( $7, $5, &quot;s.&quot;, $3, $2 );
			conout( $$, $2 );
			bundle( $1, &quot;s.&quot;, $3, $2, &quot; &quot; );
			}
	|  &#39;~&#39; LETTER &#39;=&#39; e
		={	bundle($4,&quot;S&quot;,$2); }
	;

EQOP	:  EQPL
		={ $$ = &quot;+&quot;; }
	|  EQMI
		={ $$ = &quot;-&quot;; }
	|  EQMUL
		={ $$ = &quot;*&quot;; }
	|  EQDIV
		={ $$ = &quot;/&quot;; }
	|  EQREM
		={ $$ = &quot;%%&quot;; }
	|  EQEXP
		={ $$ = &quot;^&quot;; }
	;

fprefix	:  _FOR &#39;(&#39; e &#39;;&#39;
		={ $$ = $3; }
	;

BLEV	:
		={ --bindx; }
	;

slist	:  stat
	|  slist tail stat
		={ bundle( $1, $3 ); }
	;

tail	:  &#39;\n&#39;
	|  &#39;;&#39;
	;

re	:  e EQ e
		= bundle( $1, $3, &quot;=&quot; );
	|  e &#39;&lt;&#39; e
		= bundle( $1, $3, &quot;&gt;&quot; );
	|  e &#39;&gt;&#39; e
		= bundle( $1, $3, &quot;&lt;&quot; );
	|  e NE e
		= bundle( $1, $3, &quot;!=&quot; );
	|  e GE e
		= bundle( $1, $3, &quot;!&gt;&quot; );
	|  e LE e
		= bundle( $1, $3, &quot;!&lt;&quot; );
	|  e
		= bundle( $1, &quot; 0!=&quot; );
	;

e	:  e &#39;+&#39; e
		= bundle( $1, $3, &quot;+&quot; );
	|  e &#39;-&#39; e
		= bundle( $1, $3, &quot;-&quot; );
	| &#39;-&#39; e		%prec UMINUS
		= bundle( &quot; 0&quot;, $2, &quot;-&quot; );
	|  e &#39;*&#39; e
		= bundle( $1, $3, &quot;*&quot; );
	|  e &#39;/&#39; e
		= bundle( $1, $3, &quot;/&quot; );
	|  e &#39;%&#39; e
		= bundle( $1, $3, &quot;%%&quot; );
	|  e &#39;^&#39; e
		= bundle( $1, $3, &quot;^&quot; );
	|  LETTER &#39;[&#39; e &#39;]&#39;
		={ bundle($3, &quot;;&quot;, geta($1)); }
	|  LETTER INCR
		= bundle( &quot;l&quot;, $1, &quot;d1+s&quot;, $1 );
	|  INCR LETTER
		= bundle( &quot;l&quot;, $2, &quot;1+ds&quot;, $2 );
	|  DECR LETTER
		= bundle( &quot;l&quot;, $2, &quot;1-ds&quot;, $2 );
	|  LETTER DECR
		= bundle( &quot;l&quot;, $1, &quot;d1-s&quot;, $1 );
	| LETTER &#39;[&#39; e &#39;]&#39; INCR
		= bundle($3,&quot;;&quot;,geta($1),&quot;d1+&quot;,$3,&quot;:&quot;,geta($1));
	| INCR LETTER &#39;[&#39; e &#39;]&#39;
		= bundle($4,&quot;;&quot;,geta($2),&quot;1+d&quot;,$4,&quot;:&quot;,geta($2));
	| LETTER &#39;[&#39; e &#39;]&#39; DECR
		= bundle($3,&quot;;&quot;,geta($1),&quot;d1-&quot;,$3,&quot;:&quot;,geta($1));
	| DECR LETTER &#39;[&#39; e &#39;]&#39;
		= bundle($4,&quot;;&quot;,geta($2),&quot;1-d&quot;,$4,&quot;:&quot;,geta($2));
	| SCALE INCR
		= bundle(&quot;Kd1+k&quot;);
	| INCR SCALE
		= bundle(&quot;K1+dk&quot;);
	| SCALE DECR
		= bundle(&quot;Kd1-k&quot;);
	| DECR SCALE
		= bundle(&quot;K1-dk&quot;);
	| BASE INCR
		= bundle(&quot;Id1+i&quot;);
	| INCR BASE
		= bundle(&quot;I1+di&quot;);
	| BASE DECR
		= bundle(&quot;Id1-i&quot;);
	| DECR BASE
		= bundle(&quot;I1-di&quot;);
	| OBASE INCR
		= bundle(&quot;Od1+o&quot;);
	| INCR OBASE
		= bundle(&quot;O1+do&quot;);
	| OBASE DECR
		= bundle(&quot;Od1-o&quot;);
	| DECR OBASE
		= bundle(&quot;O1-do&quot;);
	|  LETTER &#39;(&#39; cargs &#39;)&#39;
		= bundle( $3, &quot;l&quot;, getf($1), &quot;x&quot; );
	|  LETTER &#39;(&#39; &#39;)&#39;
		= bundle( &quot;l&quot;, getf($1), &quot;x&quot; );
	|  cons
		={ bundle( &quot; &quot;, $1 ); }
	|  DOT cons
		={ bundle( &quot; .&quot;, $2 ); }
	|  cons DOT cons
		={ bundle( &quot; &quot;, $1, &quot;.&quot;, $3 ); }
	|  cons DOT
		={ bundle( &quot; &quot;, $1, &quot;.&quot; ); }
	|  DOT
		={ $$ = &quot;l.&quot;; }
	|  LETTER
		= { bundle( &quot;l&quot;, $1 ); }
	|  LETTER &#39;=&#39; e
		={ bundle( $3, &quot;ds&quot;, $1 ); }
	|  LETTER EQOP e	%prec &#39;=&#39;
		={ bundle( &quot;l&quot;, $1, $3, $2, &quot;ds&quot;, $1 ); }
	|  &#39;(&#39; e &#39;)&#39;
		= { $$ = $2; }
	|  &#39;?&#39;
		={ bundle( &quot;?&quot; ); }
	|  SQRT &#39;(&#39; e &#39;)&#39;
		={ bundle( $3, &quot;v&quot; ); }
		| &#39;~&#39; LETTER
		={ bundle(&quot;L&quot;,$2); }
	| SCALE e
		= bundle($2,&quot;dk&quot;);
	| SCALE &#39;=&#39; e
		= bundle($3,&quot;dk&quot;);
	| SCALE EQOP e		%prec &#39;=&#39;
		= bundle(&quot;K&quot;,$3,$2,&quot;dk&quot;);
	| BASE e
		= bundle($2,&quot;di&quot;);
	| BASE &#39;=&#39; e
		= bundle($3,&quot;di&quot;);
	| BASE EQOP e		%prec &#39;=&#39;
		= bundle(&quot;I&quot;,$3,$2,&quot;di&quot;);
	| OBASE e
		= bundle($2,&quot;do&quot;);
	| OBASE &#39;=&#39; e
		= bundle($3,&quot;do&quot;);
	| OBASE EQOP e		%prec &#39;=&#39;
		= bundle(&quot;O&quot;,$3,$2,&quot;do&quot;);
	| SCALE
		= bundle(&quot;K&quot;);
	| BASE
		= bundle(&quot;I&quot;);
	| OBASE
		= bundle(&quot;O&quot;);
	;

cargs	:  eora
	|  cargs &#39;,&#39; eora
		= bundle( $1, $3 );
	;
eora:	  e
	| LETTER &#39;[&#39; &#39;]&#39;
		=bundle(&quot;l&quot;,geta($1));
	;

cons	:  constant
		={ *cp++ = &#39;\0&#39;; }

constant:
	  &#39;_&#39;
		={ $$ = cp; *cp++ = &#39;_&#39;; }
	|  DIGIT
		={ $$ = cp; *cp++ = $1; }
	|  constant DIGIT
		={ *cp++ = $2; }
	;

CRS	:
		={ $$ = cp; *cp++ = crs++; *cp++ = &#39;\0&#39;; bstack[bindx++] = lev++; }
	;

def	:  _DEFINE LETTER &#39;(&#39;
		={	$$ = getf($2);
			pre = &quot;&quot;;
			post = &quot;&quot;;
			lev = 1;
			bstack[bindx=0] = 0;
			}
	;

dargs	:
	|  lora
		={ pp( $1 ); }
	|  dargs &#39;,&#39; lora
		={ pp( $3 ); }
	;

dlets	:  lora
		={ tp($1); }
	|  dlets &#39;,&#39; lora
		={ tp($3); }
	;
lora	:  LETTER
	|  LETTER &#39;[&#39; &#39;]&#39;
		={ $$ = geta($1); }
	;

%%
# define error 256

int peekc -1;
int sargc;
int ifile;
char **sargv;
extern int fin;

char *funtab[26]{
	01,02,03,04,05,06,07,010,011,012,013,014,015,016,017,
	020,021,022,023,024,025,026,027,030,031,032 };
char *atab[26]{
	0241,0242,0243,0244,0245,0246,0247,0250,0251,0252,0253,
	0254,0255,0256,0257,0260,0261,0262,0263,0264,0265,0266,
	0267,0270,0271,0272};
char *letr[26] {
  &quot;a&quot;,&quot;b&quot;,&quot;c&quot;,&quot;d&quot;,&quot;e&quot;,&quot;f&quot;,&quot;g&quot;,&quot;h&quot;,&quot;i&quot;,&quot;j&quot;,
  &quot;k&quot;,&quot;l&quot;,&quot;m&quot;,&quot;n&quot;,&quot;o&quot;,&quot;p&quot;,&quot;q&quot;,&quot;r&quot;,&quot;s&quot;,&quot;t&quot;,
  &quot;u&quot;,&quot;v&quot;,&quot;w&quot;,&quot;x&quot;,&quot;y&quot;,&quot;z&quot; } ;
char *dot { &quot;.&quot; };
yylex(){
  int c,ch;
restart:
  c = getc();
  peekc = -1;
  while( c == &#39; &#39; || c == &#39;\t&#39; ) c = getc();
  if( c&lt;= &#39;z&#39; &amp;&amp; c &gt;= &#39;a&#39; ) {
    /* look ahead to look for reserved words */
    peekc = getc();
    if( peekc &gt;= &#39;a&#39; &amp;&amp; peekc &lt;= &#39;z&#39; ){ /* must be reserved word */
      if( c==&#39;i&#39; &amp;&amp; peekc==&#39;f&#39; ){ c=_IF; goto skip; }
      if( c==&#39;w&#39; &amp;&amp; peekc==&#39;h&#39; ){ c=_WHILE; goto skip; }
      if( c==&#39;f&#39; &amp;&amp; peekc==&#39;o&#39; ){ c=_FOR; goto skip; }
      if( c==&#39;s&#39; &amp;&amp; peekc==&#39;q&#39; ){ c=SQRT; goto skip; }
      if( c==&#39;r&#39; &amp;&amp; peekc==&#39;e&#39; ){ c=_RETURN; goto skip; }
      if( c==&#39;b&#39; &amp;&amp; peekc==&#39;r&#39; ){ c=_BREAK; goto skip; }
      if( c==&#39;d&#39; &amp;&amp; peekc==&#39;e&#39; ){ c=_DEFINE; goto skip; }
      if( c==&#39;s&#39; &amp;&amp; peekc==&#39;c&#39; ){ c= SCALE; goto skip; }
      if( c==&#39;b&#39; &amp;&amp; peekc==&#39;a&#39; ){ c=BASE; goto skip; }
      if( c==&#39;o&#39; &amp;&amp; peekc==&#39;b&#39; ){ c=OBASE; goto skip; }
      if( c==&#39;d&#39; &amp;&amp; peekc==&#39;i&#39; ){ c=FFF; goto skip; }
      if( c==&#39;a&#39; &amp;&amp; peekc==&#39;u&#39; ){ c=_AUTO; goto skip; }
      if( c == &#39;q&#39; &amp;&amp; peekc == &#39;u&#39;)getout();
      /* could not be found */
      return( error );
    skip:  /* skip over rest of word */
	peekc = -1;
      while( (ch = getc()) &gt;= &#39;a&#39; &amp;&amp; ch &lt;= &#39;z&#39; );
	peekc = ch;
      return( c );
      }

    /* usual case; just one single letter */

    yylval = letr[c-&#39;a&#39;];
    return( LETTER );
    }
  if( c&gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39; || c&gt;= &#39;A&#39; &amp;&amp; c&lt;= &#39;F&#39; ){
    yylval = c;
    return( DIGIT );
    }
  switch( c ){
    case &#39;.&#39;:  return( DOT );
    case &#39;=&#39;:
      switch( peekc = getc() ){
        case &#39;=&#39;: c=EQ; goto gotit;
        case &#39;+&#39;: c=EQPL; goto gotit;
        case &#39;-&#39;: c=EQMI; goto gotit;
        case &#39;*&#39;: c=EQMUL; goto gotit;
        case &#39;/&#39;: c=EQDIV; goto gotit;
        case &#39;%&#39;: c=EQREM; goto gotit;
        case &#39;^&#39;: c=EQEXP; goto gotit;
        default:   return( &#39;=&#39; );
        gotit:     peekc = -1; return(c);
        }
    case &#39;+&#39;:  return( cpeek( &#39;+&#39;, INCR, &#39;+&#39; ) );
    case &#39;-&#39;:  return( cpeek( &#39;-&#39;, DECR, &#39;-&#39; ) );
    case &#39;&lt;&#39;:  return( cpeek( &#39;=&#39;, LE, &#39;&lt;&#39; ) );
    case &#39;&gt;&#39;:  return( cpeek( &#39;=&#39;, GE, &#39;&gt;&#39; ) );
    case &#39;!&#39;:  return( cpeek( &#39;=&#39;, NE, &#39;!&#39; ) );
    case &#39;/&#39;:
	if((peekc = getc()) == &#39;*&#39;){
		peekc = -1;
		while((getc() != &#39;*&#39;) || ((peekc = getc()) != &#39;/&#39;));
		peekc = -1;
		goto restart;
	}
	else return(c);
    case &#39;&quot;&#39;:  
	       yylval = str;
	       while((c=getc()) != &#39;&quot;&#39;)*str++ = c;
	       *str++ = &#39;\0&#39;;
	       return(QSTR);
    default:   return( c );
    }
  }

cpeek( c, yes, no ){
  if( (peekc=getc()) != c ) return( no );
  else {
    peekc = -1;
    return( yes );
    }
  }

getc(){
  int ch;
loop:
  ch = (peekc &lt; 0) ? getchar() : peekc;
  peekc = -1;
  if(ch != &#39;\0&#39;)return(ch);
  if(++ifile &gt; sargc){
	if(ifile &gt;= sargc+2)getout();
	fin = dup(0);
	goto loop;
  }
close(fin);
  if((fin = open(sargv[ifile],0)) &gt;= 0)goto loop;
  yyerror(&quot;cannot open input file&quot;);
}
# define b_sp_max 1500
int b_space [ b_sp_max ];
int * b_sp_nxt { b_space };

bdebug 0;
bundle(a){
  int i, *p, *q;

  i = nargs();
  q = b_sp_nxt;

  if( bdebug ) printf(&quot;bundle %d elements at %o\n&quot;, i, q );

  for( p = &amp;a; i--&gt;0; ++p ){

    if( b_sp_nxt &gt;= &amp; b_space[b_sp_max] ) yyerror( &quot;bundling space exceeded&quot; );

    * b_sp_nxt++ = *p;
    }
  * b_sp_nxt++ = 0;
  yyval = q;
  return( q );
  }

routput(p) int *p; {
  if( bdebug ) printf(&quot;routput(%o)\n&quot;, p );
  if( p &gt;= &amp;b_space[0] &amp;&amp; p &lt; &amp;b_space[b_sp_max]){
    /* part of a bundle */
    while( *p != 0 ) routput( *p++ );
    }
  else printf( p );   /* character string */
  }

output( p ) int *p; {
  routput( p );
  b_sp_nxt = &amp; b_space[0];
  printf( &quot;\n&quot; );
  cp = cary;
  str = string;
  crs = rcrs;
  }

conout( p, s ) int *p; char *s; {
  printf(&quot;[&quot;);
  routput( p );
  printf(&quot;]s%s\n&quot;, s );
  lev--;
  str = string;
  }

yyerror( s ) char *s; {
  printf(&quot;c[%s]pc\n&quot;, s );
  cp = cary;
  crs = rcrs;
  bindx = 0;
  lev = 0;
  b_sp_nxt = &amp;b_space[0];
 str = string;
  }

pp( s ) char *s; {
  /* puts the relevant stuff on pre and post for the letter s */

  bundle( &quot;S&quot;, s, pre );
  pre = yyval;
  bundle( post, &quot;L&quot;, s, &quot;s.&quot; );
  post = yyval;
  }

tp( s ) char *s; { /* same as pp, but for temps */
  bundle( &quot;0S&quot;, s, pre );
  pre = yyval;
  bundle( post, &quot;L&quot;, s, &quot;s.&quot; );
  post = yyval;
  }

yyinit(argc,argv) int argc; char *argv[];{
  int (*getout)();
  signal( 2, getout );  /* ignore all interrupts */
  sargv=argv;
  sargc= -- argc;
  if(sargc == 0)fin=dup(0);
   else if((fin = open(sargv[1],0)) &lt; 0)
	yyerror(&quot;cannot open input file&quot;);
  ifile = 1;
  }
getout(){
printf(&quot;q&quot;);
exit();
}

getf(p) char *p;{
return(&amp;funtab[*p -0141]);
}
geta(p) char *p;{
	return(&amp;atab[*p - 0141]);
}

main(argc, argv)
char **argv;
{
	int p[2];


	if (argc &gt; 1 &amp;&amp; *argv[1] == &#39;-&#39;) {
		if(argv[1][1] == &#39;d&#39;){
			yyinit(--argc, ++argv);
			yyparse();
			exit();
		}
		if(argv[1][1] != &#39;l&#39;){
			printf(&quot;unrecognizable argument\n&quot;);
			exit();
		}
		argv[1] = &quot;/usr/lib/lib.b&quot;;
	}
	pipe(p);
	if (fork()==0) {
		close(1);
		dup(p[1]);
		close(p[0]);
		close(p[1]);
		yyinit(argc, argv);
		yyparse();
		exit();
	}
	close(0);
	dup(p[0]);
	close(p[0]);
	close(p[1]);
	execl(&quot;/bin/dc&quot;, &quot;dc&quot;, &quot;-&quot;, 0);
}
