# include &quot;ne.h&quot;

char *res[] {
	&quot;&gt;=&quot;, &quot;&lt;=&quot;, &quot;!=&quot;,
	&quot;+-&quot;, &quot;==&quot;, &quot;cdot&quot;, &quot;CDOT&quot;,
	&quot;times&quot;, &quot;TIMES&quot;,
	&quot;SIGMA&quot;, &quot;pi&quot;, &quot;PI&quot;,
	&quot;alpha&quot;, &quot;beta&quot;, &quot;gamma&quot;, &quot;GAMMA&quot;, &quot;delta&quot;, &quot;epsilon&quot;, &quot;omega&quot;,
	&quot;DELTA&quot;, &quot;LAMBDA&quot;, &quot;PHI&quot;, &quot;OMEGA&quot;,
	&quot;lambda&quot;, &quot;mu&quot;, &quot;nu&quot;, &quot;theta&quot;, &quot;rho&quot;, &quot;sigma&quot;, &quot;tau&quot;, &quot;phi&quot;,
	&quot;INF&quot;, &quot;INFINITY&quot;,
	&quot;inf&quot;, &quot;infinity&quot;,
	&quot;partial&quot;, &quot;PARTIAL&quot;,
	&quot;zeta&quot;, &quot;eta&quot;, &quot;iota&quot;, &quot;kappa&quot;, &quot;xi&quot;, &quot;omicron&quot;, &quot;upsilon&quot;,
	&quot;chi&quot;, &quot;psi&quot;, &quot;THETA&quot;, &quot;XI&quot;, &quot;UPSILON&quot;, &quot;PSI&quot;,
	&quot;del&quot;, &quot;DEL&quot;,
	&quot;nothing&quot;, &quot;NOTHING&quot;,
	&quot;approx&quot;, &quot;APPROX&quot;,
	0};
char *restran[] {
	&quot;&gt;\b_&quot;, &quot;&lt;\b_&quot;, &quot;/\b=&quot;,
	&quot;+\b_&quot;, &quot;=\b_&quot;, &quot;8.9&quot;, &quot;8.9&quot;,
	&quot;x&quot;, &quot;x&quot;,
	&quot;R&quot;, &quot;J&quot;, &quot;P&quot;,
	&quot;A&quot;, &quot;B&quot;, &quot;\\e&quot;, &quot;G&quot;, &quot;D&quot;, &quot;S&quot;, &quot;C&quot;,
	&quot;W&quot;, &quot;E&quot;, &quot;F&quot;, &quot;Z&quot;,
	&quot;L&quot;, &quot;M&quot;, &quot;@&quot;, &quot;T&quot;, &quot;K&quot;, &quot;Y&quot;, &quot;I&quot;, &quot;U&quot;,
	&quot;o&quot;, &quot;o&quot;, &quot;o&quot;, &quot;o&quot;,
	&quot;]&quot;, &quot;]&quot;,
	&quot;Q&quot;, &quot;N&quot;, &quot;i&quot;, &quot;k&quot;, &quot;X&quot;, &quot;o&quot;, &quot;u&quot;,
	&quot;X&quot;, &quot;V&quot;, &quot;O&quot;, &quot;X&quot;, &quot;U&quot;, &quot;H&quot;,
	&quot;[&quot;, &quot;[&quot;,
	&quot;&quot;, &quot;&quot;,
	&quot;~\b=&quot;, &quot;~\b=&quot;,
	0};

int	csp;
int	psp;
#define	CSSIZE	400
char	cs[420];

text(t,p1) int t; char *p1; {
	int i,j,c;
	int w;
	yyval = oalloc();
	ebase[yyval] = 0;
	eht[yyval] = 2;	/* ht in 1/2 spaces */
	if( t==&#39;q&#39; )
		j = p1;
	else if ( t == &#39;~&#39; )
		j = &amp;&quot;~&quot;;
	else if ( t == &#39;^&#39; )
		j = &amp;&quot;&quot;;
	else if ( t == &#39;\t&#39; )
		j = &amp;&quot;\\t&quot;;
	else if( (i=lookup(p1,res))&gt;=0 )
		j = restran[i];
	else {
		for( csp=psp=0; (c=p1[psp++])!=&#39;\0&#39;; ){
			trans(c,p1);
			if( csp&gt;CSSIZE )
				error(FATAL,&quot;converted token %.20s... too long&quot;,p1);
		}
		cs[csp] = &#39;\0&#39;;
		j = cs;
	}
	ewid[yyval] = width(j);
	if(dbg)printf(&quot;.\t%ctext: S%d &lt;- %s; b=%d,h=%d,w=%d\n&quot;,
		t, yyval, j, ebase[yyval], eht[yyval], ewid[yyval]);
	printf(&quot;.ds %d \&quot;%s\n&quot;,yyval,j);
}

width(s) char *s; {
	int c,w;
	w = 0;
	while( (c = *s++) != &#39;\0&#39; ){
		if( c == &#39;\b&#39; || c == 033 )
			w--;
		else if ( c == &#39;\\&#39; &amp;&amp; *s == &#39;\\&#39; );
		else if ( c == &#39;\\&#39; &amp;&amp; *s == &#39;e&#39; );
		else if ( c &gt;= 040 )
			w++;
	}
	return(w);
}

trans(c,p1) int c; char *p1; {
	switch( c){
	case &#39;&gt;&#39;: case &#39;&lt;&#39;: case &#39;=&#39;:
		if( p1[psp]==&#39;=&#39; ){	/* look ahead for == &lt;= &gt;= */
			cs[csp++] = c; cs[csp++] = &#39;\b&#39;; cs[csp++] = &#39;_&#39;;
			psp++;
		} else 
			cs[csp++] = c;
		break;
	case &#39;\\&#39;:	/* troff - pass 2 or 3 more chars */
		cs[csp++] = c; cs[csp++] = c = p1[psp++]; cs[csp++] = p1[psp++];
		if( c==&#39;(&#39; ) cs[csp++] = p1[psp++];
		break;
	default:
		cs[csp++] = c; break;
	}
}
