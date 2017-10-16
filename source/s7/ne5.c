#include &quot;ne.h&quot;

int	markpos;

mark(n) int n; {
	if( n )
		markpos = ewid[n];
	else {
		yyval = oalloc();
		printf(&quot;.ds %d \&quot;\n&quot;, yyval);
		ebase[yyval] = ewid[yyval] = markpos = 0;
		eht[yyval] = 2;
	}
	if(dbg)printf(&quot;.\tmark %d as %d\n&quot;, n, markpos);
}

lineup(n) int n; {
	if( n ) {
		printf(&quot;.ds %d \&quot;&quot;, n);
		fwd(markpos-ewid[n]);
		printf(&quot;\\*(%d\n&quot;, n);
		ewid[n] = markpos;
	}
	else {
		yyval = oalloc();
		printf(&quot;.ds %d \&quot;&quot;, yyval);
		fwd(markpos);
		printf(&quot;\n&quot;);
		ebase[yyval] = 0;
		eht[yyval] = 2;
		ewid[yyval] = markpos;
	}
	if(dbg)printf(&quot;.\tlineup %d at %d\n&quot;, n, markpos);
}
