#include &quot;ne.h&quot;

column(type, p1, p2) int type, p1, p2; {
	int i, n;
	lp[p1] = ct - p1 - 1;
	if( dbg ){
		printf(&quot;.\t%c column of&quot;, type);
		for( i=p1+1; i&lt;ct; i++ )
			printf(&quot; S%d&quot;, lp[i]);
		printf(&quot;, rows=%d\n&quot;,lp[p1]);
	}
	lp[ct++] = type;
}

matrix(p1,p2) int p1, p2; {
	int nrow, ncol, i, j, k, hb, b, val[100], db;
	int w;
	char *space;
	space = &quot;\\ \\ &quot;;
	nrow = lp[p1];	/* disaster if rows inconsistent */
	ncol = 0;
	for( i=p1; i&lt;ct; i =+ lp[i]+2 ){
		ncol++;
		if(dbg)printf(&quot;.\tcolct=%d\n&quot;,lp[i]);
	}
	for( k=1; k&lt;=nrow; k++ ) {
		hb = b = 0;
		j = p1 + k;
		for( i=0; i&lt;ncol; i++ ) {
			hb = max(hb, eht[lp[j]]-ebase[lp[j]]);
			b = max(b, ebase[lp[j]]);
			j =+ nrow + 2;
		}
		if(dbg)printf(&quot;.\trow %d: b=%d, hb=%d\n&quot;, k, b, hb);
		j = p1 + k;
		for( i=0; i&lt;ncol; i++ ) {
			ebase[lp[j]] = b;
			eht[lp[j]] = b + hb;
			j =+ nrow + 2;
		}
	}
	j = p1;
	w = 0;
	for( i=0; i&lt;ncol; i++ ) {
		lpile(lp[j+lp[j]+1], j+1, j+lp[j]+1);
		val[i] = yyval;
		w =+ ewid[yyval];
		j =+ nrow + 2;
	}
	yyval = oalloc();
	eht[yyval] = eht[val[0]];
	ebase[yyval] = ebase[val[0]];
	ewid[yyval] = w + 2 * (ncol-1);	/* 2 = width(space) */
	if(dbg)printf(&quot;.\tmatrix S%d: r=%d, c=%d, h=%d, b=%d, w=%d\n&quot;,
		yyval,nrow,ncol,eht[yyval],ebase[yyval], ewid[yyval]);
	printf(&quot;.ds %d \&quot;&quot;, yyval);
	for( i=0; i&lt;ncol; i++ )  {
		printf(&quot;\\*(%d%s&quot;, val[i], i==ncol-1 ? &quot;&quot; : space);
		ofree(val[i]);
	}
	printf(&quot;\n&quot;);
	ct = p1;
}
