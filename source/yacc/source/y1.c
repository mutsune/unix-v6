# include &quot;dextern&quot;
  /*     * * * *    y a c c     * * * *     */

  /*      features to be fixed up ...

  ***  Print estimate of total space needed for parser
  ***  Either list inputs on y.output, or list empty prdn&#39;s in states
  ***  Mention nonterms not used (or, rules. not reduced) as nonfatal error
  ***  Output states where conflicts were found by default on y.output
  ***  Engage in newspeak: production=&gt;grammar rules, term=&gt;token, etc.
  ***  handle # define, #ifdef, etc., in yacc actions, %{ %}
  */

  /*      new features to be added

  ***  reductions by single productions ( by request )
  ***  follow sets for start symbol
  ***  option to only do slr(1)
  ***  easily changed array names on output
  ***  allocate core, rather than predefined
  ***  input controlled by a grammar
  ***  support multiple choices for  conflicts
  ***  better conflict diagnostics
  */



main(argc,argv) int argc; char *argv[]; {
  auto int n;

  whereami();
  setup(argc,argv); /* initialize and read productions */
  tbitset = (nterms+16)/16;
  cpres(); /* make table of which productions yield a given nonterminal */
  cempty(); /* make a table of which nonterminals can match the empty string */
  cpfir(); /* make a table of e free first lists */
  stagen(); /* generate the states */
  output();  /* write the states and the tables */
  go2out();
  summary();
  windup();
  }

whereami(){ /* sets the variable machine to UNIX, GCOS, or IBM */

  int i;

  i = 1;
  i = i &lt;&lt; 30;
  if( i == 0 ) {
    machine = UNIX;
    return;
    }
  i = i &lt;&lt; 4;
  if( i == 0 ){
    machine = IBM;
    return;
    }
  machine = GCOS;
  }

windup(){
  /* no errors, do the optimization if appropriate */
  char *cp;
  int i;

  cflush(1);
  if( !oflag ) cexit(0);

  for( i=3; i&lt;10; ++i ) cclose(i);
  switch( machine ){

  case GCOS:
    if( rflag ){
      if( foutput&lt;0 ) system( &quot;./yopt -r&quot; );
      else system( &quot;./yopt -rv&quot; );
      }
    else {
      if( foutput&lt;0 ) system( &quot;./yopt&quot; );
      else system( &quot;./yopt -v&quot; );
      }
    cexit(0);  /* terminate */

  case UNIX:
    cp = &quot;/usr/yacc/yopti&quot;;
    if( rflag ) execl( cp, cp, (foutput&lt;0)?&quot;-r&quot;:&quot;-rv&quot;, 0 );
    else if( foutput&lt;0 ) execl( cp, cp, 0 );
    else execl( cp, cp, &quot;-v&quot;, 0 );
    error( &quot;optimization execl call fails&quot; );

  case IBM:
    if( rflag ){
      if( foutput&lt;0 ) system( &quot;MH2019.yaccopt -r&quot; );
      else system( &quot;MH2019.yaccopt -rv&quot; );
      }
    else {
      if( foutput&lt;0 ) system( &quot;MH2019.yaccopt&quot; );
      else system( &quot;MH2019.yaccopt -v&quot; );
      }
    cexit(0);

    }

  }

settty()
/*	sets the output file to y.output */
{	
	cflush( foutput );  /* a bit of a cheat */
	cout = foutput;
	}

settab(){ /* sets the output file to y.tab.c */
	
	cflush( ftable );
	cout = ftable;
	}

char *chcopy( p, q )  char *p, *q; {
	/* copies string q into p, returning next free char ptr */
	while( *p = *q++ ) ++p;
	return( p );
	}

char *writem(pp) struct item *pp; { /* creates output string for item pointed to by pp */
	int i,*p;
	static char sarr[100];
	char *q;

	for( p=pp-&gt;pitem; *p&gt;0 ; ++p ) ;
	p = prdptr[-*p];
	q = chcopy( sarr, nontrst[*p-NTBASE].name );
	q = chcopy( q, &quot; : &quot; );

	for(;;){
		*q++ = ++p==(pp-&gt;pitem) ? &#39;_&#39; : &#39; &#39;;
		if((i = *p) &lt;= 0) break;
		q = chcopy( q, symnam(i) );
		}

	*q = &#39;\0&#39; ;
	return( sarr );
	}

char *symnam(i){ /* return a pointer to the name of symbol i */
	char *cp;

	cp = (i&gt;=NTBASE) ? nontrst[i-NTBASE].name : trmset[i].name ;
	if( *cp == &#39; &#39; ) ++cp;
	return( cp );
	}

summary(){ /* output the summary on the tty */

  int i, s, *pn;
  

	if( !rflag ){
		settab();
		printf(&quot;\nint nterms %d;&quot;,nterms);
		printf(&quot;\nint nnonter %d;&quot;, nnonter);
		printf(&quot;\nint nstate %d;&quot;, nstate);
		printf(&quot;\nchar *yysterm[] {&quot;);
		for (i=1;i&lt;=nterms;i++) if( trmset[i].value &gt;= 0400 ) printf(&quot;\n\&quot;%s\&quot;,&quot;,symnam(i));
		printf( &quot;\n0 };\n&quot; );
		printf(&quot;\nchar *yysnter[] {&quot;);
		for (i=0;i&lt;nnonter;i++) printf(&quot;\n\&quot;%s\&quot;,&quot;,nontrst[i].name);
		printf(&quot;\n\&quot;%s\&quot; };\n&quot;,nontrst[nnonter].name);
		}

  settty();
  printf(&quot;\n%d/%d terminals, %d/%d nonterminals\n&quot;, nterms, tlim,
      nnonter, ntlim );
  printf(&quot;%d/%d grammar rules, %d/%d states\n&quot;, nprod, prdlim, nstate, stsize );
  printf(&quot;%d shift/reduce, %d reduce/reduce conflicts reported\n&quot;, zzsrconf, zzrrconf );
  pn = pstate[nstate+1];
  printf(&quot;%d/%d working sets used\n&quot;, zzcwset,  wssize );
  printf(&quot;memory: states,etc. %d/%d, parser %d/%d\n&quot;, pn-mem0, memsiz,
      memact, actsiz );
  printf(&quot;%d/%d distinct lookahead sets\n&quot;, nlset, lsetsz );
  printf(&quot;%d extra closures\n&quot;, zzclose - 2*nstate );
  printf(&quot;%d action entries\n&quot;, zzacent );
  printf(&quot;%d action entries saved through merging %d states\n&quot;,zzacsave,zznsave);
  printf(&quot;%d goto entries\n&quot;, zzgoent );
  printf(&quot;%d entries saved by goto default\n&quot;, zzgobest );
  if( zzsrconf!=0 || zzrrconf!=0 ){
    cflush( errfileno );
    cout = errfileno;
    printf(&quot;\nconflicts: &quot;);
    if( zzsrconf )printf( &quot;%d shift/reduce&quot; , zzsrconf );
    if( zzsrconf &amp;&amp; zzrrconf )printf( &quot;, &quot; );
    if( zzrrconf )printf( &quot;%d reduce/reduce&quot; , zzrrconf );
    printf( &quot;\n&quot; );
    }
  }

error(s,a1){ /* write out error comment */
	
	int c;
	++nerrors;
	cflush( errfileno );
	cout = errfileno;   /* set output to tty */
	printf(&quot;\n fatal error: &quot;);
	printf(s,a1);
        printf(&quot;, line %d\n&quot;, lineno );
	if( !fatfl ) return;
	summary();
	cexit(1);
	}

arrset(s) char s[]; {
	printf(&quot;\nint %s[] {0&quot;, s );
	arrndx = 1;
	}

arrval(n){
	printf(&quot;,%d&quot;,n);
	if( (++arrndx%10) == 0 ) printf(&quot;\n&quot;);
	}

arrdone(){
	printf(&quot;,-1};\n&quot;);
	}

copy(v) char *v; {	/* copy ctokn to v */
	char *p;

	p=ctokn;
	while( *v++ = *p++ );
	}

compare(v) char *v; {	/* compare ctokn with v */
	char *p;

	for( p=ctokn; ; ++p ){
		if( *p != *v++ ) return( 0 );
		if( *p == 0 ) return(1);
		}
	}

int *yalloc(n){ /* allocate n+1 words from vector mem */
	int *omem;
	omem = mem;
	mem =+ n+1;
	if(mem-mem0 &gt;= memsiz) error(&quot;memory overflow&quot;);
	return(omem);
	}

aryfil( v, n, c ) int *v,n,c; { /* set elements 0 through n-1 to c */
  int i;
  for( i=0; i&lt;n; ++i ) v[i] = c;
  }

union( a, b, c ) int *a, *b, *c; {
  /* set a to the union of b and c */
  /* a may equal b */
  /* return 1 if c is not a subset of b, 0 otherwise */

  _REGISTER int i, x, sub;

  sub = 0;
  for( i=0; i&lt;tbitset; ++i ){
    x = b[i] | c[i];
    if( x != b[i] ) sub=1;
    a[i] = x;
    }
  return( sub );
  }

prlook( pp ) int *pp;{
	int j;
	pp = pp-&gt;lset;
	if( pp == 0 ) printf(&quot;\tNULL&quot;);
	else {
		printf(&quot; { &quot; );
		for( j=1; j&lt;=nterms; ++j ){
			if( (pp[j&gt;&gt;4]&gt;&gt;(j&amp;017) )&amp;01 != 0 ) printf( &quot;%s &quot;, symnam(j) );
			}
		printf( &quot;}&quot; );
		}
	}
