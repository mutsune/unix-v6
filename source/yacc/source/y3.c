# include &quot;dextern&quot;

cpres(){ /* conpute an array with the beginnings of  productions yielding given nonterminals
	The array pres points to these lists */
	int i,j,c;
	pres = yalloc(nnonter+1);
	for(i=0;i&lt;=nnonter;i++){
		c = i+NTBASE;
		pres[i] = mem;
		fatfl = 0;  /* make undefined  symbols  nonfatal */
		for(j=0;j&lt;nprod;j++)
		if (*prdptr[j] == c) *mem++ =  prdptr[j]+1;
		if(pres[i] == mem){
			error(&quot;nonterminal %s not defined!&quot;, nontrst[i].name);
			}
		}
	pres[i] = mem;
	fatfl = 1;
	if( nerrors ){
		summary();
		cexit(1);
		}
	}

int indebug 0;
cpfir() {
  /* compute an array with the first of nonterminals */
  int i, ch, **s, **t, changes, *p;

  pfirst = yalloc(nnonter+1);
  for (i=0;i&lt;=nnonter;i++) {
    aryfil( wsets[i].ws, tbitset, 0 );
    t = pres[i+1];
    for( s=pres[i]; s&lt;t; ++s ){ /* initially fill the sets */
      for( p = *s; (ch = *p) &gt; 0 ; ++p ) {
        if( ch &lt; NTBASE ) {
          wsets[i].ws[ch&gt;&gt;4] =| (1 &lt;&lt; (ch&amp;017) );
          break;
          }
        else if( !pempty[ch-NTBASE] ) break;
        }
      }
    }

  /* now, reflect transitivity */

  changes = 1;
  while( changes ){
    changes = 0;
    for( i=0; i&lt;=nnonter; ++i ){
      t = pres[i+1];
      for( s=pres[i]; s&lt;t; ++s ){
        for( p = *s; ( ch = (*p-NTBASE) ) &gt;= 0; ++p ) {
          changes =| union( wsets[i].ws, wsets[i].ws, wsets[ch].ws );
          if( !pempty[ch] ) break;
          }
        }
      }
    }

  for( i=0; i&lt;=nnonter; i++ ) pfirst[i] = flset( wsets[i].ws );
  if( !indebug ) return;
  settty();
  for( i=0; i&lt;=nnonter; i++ ){
    printf( &quot;\n%s: &quot;, nontrst[i].name );
    prlook( pfirst[i] );
    printf( &quot; %d\n&quot;, pempty[i] );
    }
  }

state(c){ /* sorts last state,and sees if it equals earlier ones. returns state number */
	int s,size1,size2;
	_REGISTER i;
	struct item *p1, *p2, *k, *l, *q1, *q2;
	p1 = pstate[nstate];
	p2 = pstate[nstate+1];
	if(p1==p2) return(0); /* null state */
	/* sort the items */
	for(k=p2-1;k&gt;p1;k--) {	/* make k the biggest */
		for(l=k-1;l&gt;=p1;--l)if( l-&gt;pitem &gt; k-&gt;pitem ){
			s = k-&gt;pitem;
			k-&gt;pitem = l-&gt;pitem;
			l-&gt;pitem = s;
			s = k-&gt;look;
			k-&gt;look = l-&gt;look;
			l-&gt;look = s;
			}
		}
	size1 = p2 - p1; /* size of state */

	for( i= (c&gt;=NTBASE)?ntstates[c-NTBASE]:tstates[c]; i != 0; i = mstates[i] ) {
		/* get ith state */
		q1 = pstate[i];
		q2 = pstate[i+1];
		size2 = q2 - q1;
		if (size1 != size2) continue;
		k=p1;
		for(l=q1;l&lt;q2;l++) {
			if( l-&gt;pitem != k-&gt;pitem ) break;
			++k;
			}
		if (l != q2) continue;
		/* found it */
		pstate[nstate+1] = pstate[nstate]; /* delete last state */
		/* fix up lookaheads */
		k=p1;
		for( l=q1; l&lt;q2; ++l ){
			if( union( clset.lset, l-&gt;look-&gt;lset, k-&gt;look-&gt;lset ) ) {
				tystate[i] = 1;
				/* register the new set */
				l-&gt;look = flset( &amp;clset );
				}
			++k;
			}
		return (i);
		}
/* state is new */
	pstate[nstate+2] = p2;
	if(nstate+1 &gt;= stsize) error(&quot;too many states&quot;);
	if( c &gt;= NTBASE ){
		mstates[ nstate ] = ntstates[ c-NTBASE ];
		ntstates[ c-NTBASE ] = nstate;
		}
	else {
		mstates[ nstate ] = tstates[ c ];
		tstates[ c ] = nstate;
		}
	tystate[nstate]=1;
	return(nstate++);
	}

int pidebug 0; /* debugging flag for putitem */
putitem ( ptr, lptr )		int *ptr; struct looksets *lptr;{
	int *jip, k;
	struct item *i, *j;

        if( pidebug ) {
          settty();
	  printf(&quot;putitem(%s), state %d\n&quot;, writem(&amp;ptr), nstate );
          }
	/* see if it&#39;s there */
	j = pstate[nstate+1];
        for( i=pstate[nstate]; i&lt;j; ++i )
		if(i-&gt;pitem == ptr) { 
			error(&quot;yacc error--duplicate item&quot;);
			}
	/* not there */
	j-&gt;pitem = ptr;
	j-&gt;look = flset( lptr );
	pstate[nstate+1] = ++j;
	jip = j;
	if(jip-mem0 &gt;= memsiz) error(&quot;out of state space&quot;);
	}

cempty(){ /* mark nonterminals which derive the empty string */

  int i, *p;

  /* set pempty to 0 */
  pempty = yalloc( nnonter );
  aryfil( pempty, nnonter+1, 0 );
  for( i=1; i&lt;nprod; ++i ) /* loop over productions */
    if( prdptr[i][1] &lt;= 0 ) /* i is empty production */
      pempty[ *prdptr[i]-NTBASE ] = 1;

  /* now, all explicitly empty nonterminals marked with pempty = 1 */

  /* loop as long as we keep finding nontrivial empty nonterminals */

again:
  for( i=1; i&lt;nprod; ++i ){
    if( pempty[ *prdptr[i]-NTBASE ]==0 ){ /* not known to be empty */
      for( p=prdptr[i]+1; *p&gt;=NTBASE &amp;&amp; pempty[*p-NTBASE]!=0 ; ++p ) ;
      if( *p &lt; 0 ){ /* we have a nontrivially empty nonterminal */
        pempty[*prdptr[i]-NTBASE] = 1;
        goto again; /* got one ... try for another */
        }
      }
    }
  }

int gsdebug 0;
stagen(){ /* generate the states */

  int i, j, k, c;

  /* initialize */

  nstate = 0;
  pstate[0] = pstate[1] = mem;
  aryfil( clset.lset, tbitset, 0 );
  putitem( prdptr[0]+1, &amp;clset );
  tystate[0] = 1;
  nstate = 1;
  pstate[2] = pstate[1];

  memact = 0;
  aryfil( amem, actsiz, 0 );

  /* now, the main state generation loop */

  more:
  for( i=0; i&lt;nstate; ++i ){
    if( tystate[i] != 1 ) continue;
    tystate[i] = 0;
    aryfil( temp1, nterms+nnonter+1, 0 );
    /* take state i, close it, and do gotos */
    closure(i);
    for( j=0; j&lt;cwset; ++j ){ /* generate gotos */
      if( wsets[j].flag ) continue;
      wsets[j].flag = 1;
      c = *(wsets[j].pitem);
      if( c &lt;= 1 ) {
		if( pstate[i+1]-pstate[i] &lt;= j ) tystate[i] = (tystate[i]==1)?1:2;
		continue;
		}
      /* do a goto on c */
      for( k=j; k&lt;cwset; ++k ){
        if( c == *(wsets[k].pitem) ){ /* this item contributes to the goto */
          putitem( wsets[k].pitem + 1, wsets[k].ws );
          wsets[k].flag = 1;
          }
        }
      if( c &lt; NTBASE ) temp1[c] = state(c);
      else temp1[c-NTBASE+nterms] = state(c);
      }
    if( gsdebug ){
      settty();
      printf( &quot;%d: &quot;, i );
      for( j=1; j&lt;=nterms; ++j ){
        if( temp1[j] != 0 ) printf( &quot;%s %d, &quot;, symnam(j), temp1[j]);
        }
      for( j=1; j&lt;=nnonter; ++j ){
        if( temp1[j+nterms] ) printf( &quot;%s %d, &quot;, nontrst[j].name, temp1[j+nterms] );
        }
      printf(&quot;\n&quot;);
      }
    apstate[i] = apack( &amp;temp1[0], nterms );
    indgo[i] = apack( &amp;temp1[nterms+1], nnonter-1 ) - 1;
    goto more; /* we have done one goto; do some more */
    }
  /* no more to do... stop */
  }

int cldebug 0; /* debugging flag for closure */
closure(i){ /* generate the closure of state i */

  int c, ch, work;
  _REGISTER j, k;
  int *pi;
  int **s, **t;
  struct item *q;
  _REGISTER struct item *p;

  ++zzclose;

  /* first, copy kernel of state i to wsets */

  cwset = 0;
  q = pstate[i+1];
  for( p=pstate[i]; p&lt;q; ++p ){
    wsets[cwset].pitem = p-&gt;pitem;
    wsets[cwset].flag = 1;    /* this item must get closed */
    for( k=0; k&lt;tbitset; ++k ) wsets[cwset].ws[k] = p-&gt;look-&gt;lset[k];
    ++cwset;
    }

  /* now, go through the loop, closing each item */

  work = 1;
  while( work ){
    work = 0;
    for( j=0; j&lt;cwset; ++j ){
  
      if( wsets[j].flag == 0 ) continue;
      c = *(wsets[j].pitem);  /* dot is before c */
  
      if( c &lt; NTBASE ){
        wsets[j].flag = 0;
        continue;  /* only interesting case is where . is before nonterminal */
        }
  
      /* compute the lookahead */
      aryfil( clset.lset, tbitset, 0 );

      /* find items involving c */

      for( k=j; k&lt;cwset; ++k ){
        if( wsets[k].flag == 1 &amp;&amp; *(pi=wsets[k].pitem) == c ){
          wsets[k].flag = 0;
          if( nolook ) continue;
          while( (ch= *++pi)&gt;0 ){
            if( ch &lt; NTBASE ){ /* terminal symbol */
              clset.lset[ch&gt;&gt;4] =| (1&lt;&lt;(ch&amp;017));
              break;
              }
            /* nonterminal symbol */
            union( clset.lset, clset.lset, pfirst[ch-NTBASE] );
            if( !pempty[ch-NTBASE] ) break;
            }
          if( ch&lt;=0 ) union( clset.lset, clset.lset, wsets[k].ws );
          }
        }
  
      /*  now loop over productions derived from c */
  
      c =- NTBASE; /* c is now nonterminal number */
  
      t = pres[c+1];
      for( s=pres[c]; s&lt;t; ++s ){
        /* put these items into the closure */
        for( k=0; k&lt;cwset; ++k ){ /* is the item there */
          if( wsets[k].pitem == *s ){ /* yes, it is there */
            if( nolook ) goto nexts;
            if( union( wsets[k].ws, wsets[k].ws, clset.lset ) ) wsets[k].flag = work = 1;
            goto nexts;
            }
          }
  
        /*  not there; make a new entry */
        if( cwset+1 &gt;= wssize ) error( &quot;working set overflow&quot; );
        wsets[cwset].pitem = *s;
        wsets[cwset].flag = 1;
        if( nolook ){
          cwset++;
          goto nexts;
          }
        work = 1;
        for( k=0; k&lt;tbitset; ++k ) wsets[cwset].ws[k] = clset.lset[k];
        cwset++;
  
      nexts: ;
        }
  
      }
    }

  /* have computed closure; flags are reset; return */

  if( cwset &gt; zzcwset ) zzcwset = cwset;
  if( !cldebug ) return;
  settty();
  printf(&quot;\nState %d, nolook = %d\n&quot;, i, nolook );
  for( j=0; j&lt;cwset; ++j ){
    if( wsets[j].flag ) printf(&quot;flag set!\n&quot;);
    wsets[j].flag = 0;
    printf(&quot;\t%s&quot;, writem(&amp;wsets[j].pitem));
    prlook( wsets[j].ws );
    printf( &quot;\n&quot; );
    }

  }

struct looksets *flset( p ) 
	struct looksets *p;
	{
	/* decide if the lookahead set pointed to by p is known */
	/* return pointer to a perminent location for the set */

	int j, *w;
	_REGISTER *u, *v, i;

	for( i=0; i&lt;nlset; ++i ){
		u = p-&gt;lset;
		v = lkst[i].lset;
		w = &amp; v[tbitset];
		while( v&lt;w) if( *u++ != *v++ ) goto more;
		/* we have matched */
		return( &amp; lkst[i] );
		more: ;
		}
	/* add a new one */
	if( nlset+1 &gt;= lsetsz )error(&quot;too many lookahead sets&quot;);
	for( j=0; j&lt;tbitset; ++j ){
		lkst[nlset].lset[j] = p-&gt;lset[j];
		}
	return( &amp; lkst[nlset++]);
	}
