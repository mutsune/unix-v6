#include &quot;r.h&quot;

char	scrat[500];

int	brkptr	-1;
int	brkstk[10];
int	forptr	0;
int	forstk[10];

repcode() {
	outcont(0);
	yyval = genlab();
	outcont(yyval);
	brkstk[++brkptr] = yyval+1;
	genlab();
	genlab();
}

untils(p1) int p1; {
	outnum(p1+1);
	outcode(&quot;\tif(.not.&quot;);
	balpar(scrat);
	outcode(scrat);
	outcode(&quot;)&quot;);
	outgoto(p1);
	outcont(p1+2);
	brkptr--;
}

ifcode(p1) int p1; {
	outcode(&quot;\tif(.not.&quot;);
	balpar(scrat);
	outcode(scrat);
	outcode(&quot;)&quot;);
	outgoto(yyval=genlab()); genlab();
}

whilecode(p1) int p1; {
	outcont(0);
	brkstk[++brkptr] = yyval = genlab(); genlab();
	outnum(yyval);
	outcode(&quot;\tif(.not.&quot;);
	balpar(scrat);
	outcode(scrat);
	outcode(&quot;)&quot;);
	outgoto(yyval+1);
}

whilestat(p1) int p1; {
	outgoto(p1);
	outcont(p1+1);
	brkptr--;
}

balpar(bp) char *bp; {
	int  i, c, lpar;
	extern int peek;
	while( (c=getc()) == &#39; &#39; || c == &#39;\t&#39; || c==&#39;\n&#39; );
	peek = c;
	if( c != &#39;(&#39; ){
		error(&quot;missing left paren&quot;);
		bp[0] = &#39;\0&#39;;
		return(bp);
	}
	for( lpar=i=0; (bp[i++]=c=getc())!=&#39;\0&#39;; ){
		if( c==&#39;\&#39;&#39; || c==&#39;&quot;&#39; )
			while( (bp[i++]=getc()) != c );
		if( i&gt;=499 || c==&#39;{&#39; || c==&#39;}&#39; ){
			error(&quot;missing right parenthesis at %.20s&quot;, bp);
			break;
		}
		if( c==&#39;(&#39; )
			lpar++;
		else if( c==&#39;)&#39; )
			lpar--;
		if( lpar == 0 )
			break;
	}
	bp[i] = &#39;\0&#39;;
	return(bp);
}

int	labval	23000;

genlab(){
	return(++labval);
}

gokcode(p1) char *p1; {
	outcode(&quot;\t&quot;);
	outcode(p1);
	eatup(p1,scrat);
	outcode(scrat);
	outcode(0);
}

eatup(p1,bp) char *p1, *bp; {
	extern int peek;
	int i,c,lnb,lpar;
	lnb = &#39;\n&#39;;
	while( c = *p1++ )
		if( c!=&#39; &#39; )
			lnb = c;
	i = lpar = 0;
  more:
	for( ; (bp[i++]=c=getc())!=&#39;;&#39; &amp;&amp; c!=&#39;{&#39; &amp;&amp; c!=&#39;\n&#39; &amp;&amp; c!=&#39;}&#39;; ){
		if( i&gt;=499 ){
			error(&quot;statement too long at %.20s&quot;, bp);
			break;
		}
		if( c != &#39; &#39; &amp;&amp; c != &#39;\t&#39; )
			lnb = c;
		if( c==&#39;\&#39;&#39; || c==&#39;&quot;&#39; )
			while( (bp[i++]=getc()) != c );
		if( c==&#39;(&#39; )
			lpar++;
		else if( c==&#39;)&#39; ) {
			lpar--;
			if( lpar &lt; 0 )
				error(&quot;missing left paren at %.20s&quot;,bp);
		}
	}
	if( c == &#39;\n&#39; ){
		if( lnb==&#39;\n&#39; || lnb==&#39;+&#39; || lnb==&#39;-&#39; || lnb==&#39;*&#39; || lnb==&#39;(&#39;
			|| lnb==&#39;/&#39; || lnb==&#39;,&#39; || lnb==&#39;&amp;&#39;  || lnb==&#39;|&#39;
			|| lnb==&#39;=&#39; )
				goto more;
		c = &#39;;&#39;;
	}
	if( c!=&#39;;&#39; )
		peek = c;
	bp[i-1] = &#39;\0&#39;;
	if( lpar &gt; 0 )
		error(&quot;missing right paren at %.20s&quot;,bp);
	return(bp);
}

forcode(){
	extern int peek;
	int i,j,c;
	char *bp, *getvec();
	outcont(0);
	balpar(scrat);
	yyval = genlab(); genlab(); genlab();
	brkstk[++brkptr] = yyval+1;
	if( scrat[0] == &#39;\0&#39; ){
		forstk[forptr++] = bp = getvec(1);
		*bp = &#39;\0&#39;;
		return;
	}
	scrat[0] = &#39;\t&#39;;
	for( i=1; (c=scrat[i++])!=&#39;;&#39; &amp;&amp; c!=&#39;\0&#39; ; )
		if( c==&#39;\&#39;&#39; || c==&#39;&quot;&#39; )
			while( scrat[i++] != c );
	scrat[i-1] = &#39;\0&#39;;
	if( nonblank(scrat) ){
		outcode(scrat);
		outcode(0);
	}
	for( j=i; (c=scrat[i++])!=&#39;;&#39; &amp;&amp; c!=&#39;\0&#39; ; )
		if( c==&#39;\&#39;&#39; || c==&#39;&quot;&#39; )
			while( scrat[i++] != c );
	scrat[i-1] = &#39;\0&#39;;
	if( nonblank(&amp;scrat[j]) ){
		outnum(yyval);
		outcode(&quot;\tif(.not.(&quot;);
		outcode(&amp;scrat[j]);
		outcode(&quot;))&quot;);
		outgoto(yyval+2);
	}
	else
		outcont(yyval);
	for( j=0; scrat[i+1]!=&#39;\0&#39;; )
		scrat[j++] = scrat[i++];
	scrat[j] = &#39;\0&#39;;
	forstk[forptr++] = bp = getvec(j+1);
	for(i=0; *bp++ = scrat[i++]; );
}

forstat(p1) int p1; {
	char *bp, *q;
	int i;
	bp = forstk[--forptr];
	outnum(p1+1);
	if( nonblank(bp) ){
		outcode(&quot;\t&quot;);
		outcode(bp);
		outcode(0);
	}
	outgoto(p1);
	outcont(p1+2);
	for( q=bp; *q++; );
	relvec(bp, q-bp);
	brkptr--;
}

docode(new,p1) int new; char *p1; {
	outcode(&quot;\t&quot;);
	outcode(p1);
	eatup(p1,scrat);
	yyval = 0;
	if(new){
		yyval = genlab(); genlab();
		brkstk[++brkptr] = yyval;
		outnum(yyval);
	}
	outcode(scrat);
	outcode(0);
}

dostat(p1) int p1; {
	if( p1==0 )
		return;
	outcont(p1);
	outcont(p1+1);
	brkptr--;
}

breakcode(p1) int p1; {
	if(brkptr&lt;0){
		error(&quot;illegal BREAK&quot;);
		return;
	}
	outgoto(brkstk[brkptr]+1);
}

nextcode(p1) int p1; {
	if(brkptr&lt;0){
		error(&quot;illegal NEXT&quot;);
		return;
	}
	outgoto(brkstk[brkptr]);
}

nonblank(s) char *s; {
	int c;
	while( c = *s++ )
		if( c!=&#39; &#39; &amp;&amp; c!=&#39;\t&#39; &amp;&amp; c!=&#39;\n&#39; )
			return(1);
	return(0);
}

error(s1, s2) char *s1, *s2; {
	extern int linect[],ninclude,infile;
	printf( 2, &quot;error at line %d, file %d: &quot;,linect[ninclude],infile);
	printf( 2, s1,s2);
	printf( 2, &quot;\n&quot;);
	errorflag = 1;
}

errcode(p1) char *p1; {
	int c;
	extern int yychar;
	extern int linect[],ninclude,infile;
	printf( 2, &quot;\nsyntax error, line %d, file %d\n&quot;, linect[ninclude],infile);
	while( (c=getc()) != &#39;;&#39; &amp;&amp; c != &#39;}&#39; &amp;&amp; c != &#39;\n&#39; &amp;&amp; c != &#39;\0&#39; );
	yychar = -1;
	errorflag = 1;
}
