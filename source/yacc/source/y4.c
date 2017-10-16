# include &quot;dextern&quot;

output(){ /* print the output for the states */

  int i, j, k, c;

  settab();
  arrset(&quot;yyact&quot;);

  for( i=0; i&lt;nstate; ++i ){ /* output the stuff for state i */
    nolook = (tystate[i]==0);
    closure(i);
    /* output actions */
    aryfil( temp1, nterms+1, 0 );
    for( j=0; j&lt;cwset; ++j ){ /* look at the items */
      c = *( wsets[j].pitem );
      if( c&gt;0 &amp;&amp; c&lt;NTBASE &amp;&amp; temp1[c]==0 ) temp1[c] = go2(i,c);
      }

    if( i == 1 ) temp1[1] = ACCEPTCODE;

    /* now, we have the shifts; look at the reductions */

    lastred = 0;
    for( j=0; j&lt;cwset; ++j ){
      c = *( wsets[j].pitem );
      if( c&lt;=0 ){ /* reduction */
        lastred = -c;
        for( k=1; k&lt;=nterms; ++k ){
          if( ((wsets[j].ws[k&gt;&gt;4])&amp;(1&lt;&lt;(k&amp;017))) != 0 ) {
            if( temp1[k] == 0 ) temp1[k] = c;
            else if( temp1[k]&lt;0 ){ /* reduce/reduce conflict */
              settty();
              printf(&quot;\n%d: reduce/reduce conflict (red&#39;ns %d and %d ) on %s&quot;,
                i, -temp1[k], lastred, symnam(k) );
              if( -temp1[k] &gt; lastred ) temp1[k] = -lastred;
              ++zzrrconf;
              settab();
              }
            else { /* potential shift/reduce conflict */
              switch( precftn( lastred, k ) ) {

            case 0: /* precedence does not apply */

                settty();
                printf(&quot;\n%d: shift/reduce conflict (shift %d, red&#39;n %d) on %s&quot;, i,
			temp1[k], lastred, symnam(k) );
                ++zzsrconf;
                settab();
                break;

            case 1: /*  reduce */

                temp1[k] = -lastred;
                break;

            case 2: /* error, binary operator */

                temp1[k] = ERRCODE;
                break;

            case 3: /* shift ... leave the entry alone */

                break;
                }
              }
            }
          }
        }
      }
    wract(i);
    }

  settab();
  arrdone();

  /* now, output the pointers to the action array */
  /* also output the info about reductions */
  prred();
  }

prred(){ /* print the information about the actions and the reductions */
  int index, i;

  arrset(&quot;yypact&quot;);
  index = 1;    /* position in the output table */

  for( i=0; i&lt;nstate; ++i ){
    if( tystate[i]&gt;0 ){  /* the state is real */
      temp1[i] = index;
      arrval( index );
      index =+ tystate[i];
      }
    else {
      arrval( temp1[-tystate[i]] );
      }
    }

  arrdone();

  arrset(&quot;yyr1&quot;);
  for( i=1; i&lt;nprod; ++i ) arrval( *prdptr[i] - NTBASE );
  arrdone();

  arrset(&quot;yyr2&quot;);
  for( i=1; i&lt;nprod; ++i ) arrval( ( prdptr[i+1]-prdptr[i]-2 ) );
  arrdone();

  }

go2(i,c){ /* do a goto on the closure state, not worrying about lookaheads */
  if( c&lt;NTBASE ) return( amem[ apstate[i]+c ] );
  else return( amem[ apstate[i] + c - NTBASE + nterms ] );
  }

int pkdebug 0;
apack(p, n ) int *p;{ /* pack state i from temp1 into amem */
  _REGISTER k, l, off;
  int j;

  /* find the spot */

  j = n;
  for( off = 0; off &lt;= j &amp;&amp; p[off] == 0; ++off ) ;
  if( off &gt; j ){ /* no actions */
    return(0);
    }
  j =- off;
  for( k=0; k&lt;actsiz; ++k ){
    for( l=0; l&lt;=j; ++l ){
      if( p[off+l] != 0 ){
        if( p[off+l] != amem[k+l] &amp;&amp; amem[k+l] != 0 ) goto nextk;
        }
      }
    if( pkdebug ){ settty(); printf(&quot;off = %d, k = %d\n&quot;, off, k ); }
    /* we have found an acceptable k */
    for( l=0; l&lt;=j; ++l ){
      if( p[off+l] ){
        if( k+l &gt;= actsiz ) error(&quot;action table overflow&quot;);
        if( k+l &gt;= memact ) memact = k+l;
        amem[k+l] = p[off+l];
        }
      }
    if( pkdebug ){
      for( k=0; k&lt;memact; k=+10){
        printf(&quot;\t&quot;);
        for( l=0; l&lt;=9; ++l ) printf(&quot;%d &quot;, amem[k+l] );
        printf(&quot;\n&quot;);
        }
      }
    return(k-off);

    nextk: ;
    }
  error(&quot;no space in action table&quot;);
  }

go2out(){ /* output the gotos for the nontermninals */
  int i, j, k, best, offset, count, cbest, times;

  settab();
  arrset(&quot;yygo&quot;);
  offset = 1;

  for( i=1; i&lt;=nnonter; ++i ) {
    go2gen(i);

    /* find the best one to make default */

    temp2[i] = offset;

    best = -1;
    times = 0;

    for( j=0; j&lt;=nstate; ++j ){ /* is j the most frequent */
      if( tystate[j] == 0 ) continue;
      if( tystate[j] == best ) continue;

      /* is tystate[j] the most frequent */

      count = 0;
      cbest = tystate[j];

      for( k=j; k&lt;=nstate; ++k ) if( tystate[k]==cbest ) ++count;

      if( count &gt; times ){
        best = cbest;
        times = count;
        }
      }

    /* best is now the default entry */

    zzgobest =+ (times-1)*2;
    settab();
    for( j=0; j&lt;=nstate; ++j ){
      if( tystate[j] != 0 &amp;&amp; tystate[j]!=best ){
        arrval( j );
        arrval( tystate[j] );
        offset =+ 2;
        zzgoent =+ 2;
        }
      }

    /* now, the default */

    zzgoent =+ 2;
    arrval( -1 );
    arrval( best );
    offset =+ 2;

    }

  arrdone();

  arrset(&quot;yypgo&quot;);
  for( i=1; i&lt;=nnonter; ++i ) arrval( temp2[i] );
  arrdone();

  }

int g2debug 0;
go2gen(c){ /* output the gotos for nonterminal c */

  int i, work, cc;
  struct item *p, *q;

  /* first, find nonterminals with gotos on c */

  aryfil( temp1, nnonter+1, 0 );
  temp1[c] = 1;

  work = 1;
  while( work ){
    work = 0;
    for( i=0; i&lt;nprod; ++i ){
      if( (cc=prdptr[i][1]-NTBASE) &gt;= 0 ){ /* cc is a nonterminal */
        if( temp1[cc] != 0 ){ /* cc has a goto on c */
          cc = *prdptr[i]-NTBASE; /* thus, the left side of production i does too */
          if( temp1[cc] == 0 ){
            work = 1;
            temp1[cc] = 1;
            }
          }
        }
      }
    }

  /* now, we have temp1[c] = 1 if a goto on c in closure of cc */

  if( g2debug ){
    settty();
    printf(&quot;%s: gotos on &quot;, nontrst[c].name );
    for( i=0; i&lt;=nnonter; ++i ) if( temp1[i]) printf(&quot;%s &quot;, nontrst[i].name);
    printf(&quot;\n&quot;);
    }

  /* now, go through and put gotos into tystate */

  aryfil( tystate, nstate, 0 );
  settty();
  printf(&quot;\nnonterminal %s\n&quot;, nontrst[c].name );
  for( i=0; i&lt;nstate; ++i ){
    q = pstate[i+1];
    for( p=pstate[i]; p&lt;q; ++p ){
      if( (cc= *p-&gt;pitem) &gt;= NTBASE ){
        if( temp1[cc =- NTBASE] ){ /* goto on c is possible */
          tystate[i] = amem[indgo[i]+c];
          break;
          }
        }
      }
    if( tystate[i] ) printf(&quot;\t%d\t%d\n&quot;, i, tystate[i]);
    }
  }

precftn(r,t){ /* decide a shift/reduce conflict by precedence.
			Returns 0 if action is &#39;do nothing&#39;,1 if action is reduce,
			2 if the action is &#39;error,binary operator&#39;
			and 3 if the action is &#39;reduce&#39;. */

	int lp,lt;
	lp = levprd[r];
	lt = trmlev[t];
	if ((lt==0)||((lp&amp;03)==0))return(0);
	if((lt&gt;&gt;3) == (lp&gt;&gt;3)){
		return(lt&amp;03);
		}
	if((lt&gt;&gt;3) &gt; (lp&gt;&gt;3)) return(3);
	return(1);
	}

int cdebug 0; /* debug for common states */
wract(i){ /* output state i */
  /* temp1 has the actions, lastred the default */
  int p, p0, p1, size;
  int ntimes, tred, count, j;
  struct item *q0, *q1;

  /* find the best choice for lastred */

  lastred = 0;
  ntimes = 0;
  for( j=1; j&lt;=nterms; ++j ){
    if( temp1[j] &gt;= 0 ) continue;
    if( temp1[j]+lastred == 0 ) continue;
    /* count the number of appearances of temp1[j] */
    count = 0;
    tred = -temp1[j];
    for( p=1; p&lt;=nterms; ++p ){
      if( temp1[p]+tred == 0 ) ++count;
      }
    if( count &gt;ntimes ){
      lastred = tred;
      ntimes = count;
      }
    }

    /* clear out entries in temp1 which equal lastred */
    for( p=1; p&lt;= nterms; ++p ) if( temp1[p]+lastred == 0 )temp1[p]=0;

    /* write out the state */

    /* first, check for equality with another state */
    /* see if there is a nonterminal with all dots before it. */

    p0 = 0;
    q1 = pstate[i+1];
    for( q0=pstate[i]; q0&lt;q1; ++q0 ){
      if( (p1= *(q0-&gt;pitem) ) &lt; NTBASE ) goto standard;
      if( p0 == 0 ) p0 = p1;
      else if( p0 != p1 ) goto standard;
      }

    /* now, all items have dots before p0 */
    if( cdebug ){
      settty();
      printf(&quot;state %d, pre-nonterminal %s\n&quot;,i,nontrst[p0-NTBASE].name);
      }

    for( j=0; j&lt;i; ++j ){
      if( apstate[i] != apstate[j] ) continue;

      /* equal positions -- check items */

      if( cdebug )printf(&quot;states %d and %d have equal positions\n&quot;,i,j);
      q1 = pstate[j+1];
      for( q0=pstate[j]; q0&lt;q1; ++q0 ){
        if( *(q0-&gt;pitem) != p0 ) goto nextj;
        }

      /* we have a match with state j ! */

      tystate[i] = -j;
      zzacsave =+ tystate[j];
      zznsave++;
      wrstate(i);
      return;

    nextj:  ;
      }


  standard:
    tystate[i] = 2;
    wrstate(i);

    size = 0;
    for( p0=1; p0&lt;=nterms; ++p0 )
      if( (p1=temp1[p0])!=0 ) {
        arrval( TESTACT+trmset[p0].value );
        if( p1 &lt; 0 ) arrval( REDUCACT - p1 );
        else if( p1 == ACCEPTCODE ) arrval( ACCEPTACT );
        else if( p1 == ERRCODE ) arrval( ERRACT );
        else arrval( SHIFTACT + p1 );
        size =+ 2;
        }
    if( lastred ) arrval( REDUCACT + lastred );
    else arrval( ERRACT );
    tystate[i] = size+1; /* store entry size in tystate */
    zzacent =+ (size+1);
    return;
  }

wrstate(i){ /* writes state i */
	int j0,j1,s;
        struct item *pp, *qq;
	settty();
	printf(&quot;\nstate %d\n&quot;,i);
	qq = pstate[i+1];
	for( pp=pstate[i]; pp&lt;qq; ++pp) printf(&quot;\t%s\n&quot;, writem(pp));

        /* check for state equal to another */

        if( tystate[i] &lt;= 0 ){
          printf(&quot;\n\tsame as %d\n\n&quot;, -tystate[i] );
          return;
          }

	for( j0=1; j0&lt;=nterms; ++j0 ) if( (j1=temp1[j0]) != 0 ){
	printf(&quot;\n\t%s  &quot;, symnam(j0) );
             if( j1&gt;0 ){ /* shift, error, or accept */
               if( j1 == ACCEPTCODE ) printf( &quot;accept&quot; );
               else if( j1 == ERRCODE ) printf( &quot;error&quot; );
               else printf( &quot;shift %d&quot;, j1 );
               }
		   else printf(&quot;reduce %d&quot;,-j1 );
	   }

	/* output the final production */

	if( lastred ) printf(&quot;\n\t.  reduce %d\n\n&quot;, lastred );
	else printf(&quot;\n\t.  error\n\n&quot; );

ret:
	settab();
	}
