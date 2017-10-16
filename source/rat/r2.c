#include &quot;r.h&quot;

char	outbuf[80];
int	outp	0;
int	cont	0;

outcode(p) char *p; {
	int i,j,c,c1;
	char *q;
	if( p == 0 ){
		outbuf[outp] = &#39;\0&#39;;
		printf(&quot;%s\n&quot;, outbuf);
		outp = cont = 0;
		return;
	}
	while( (c = *p++) ){
		c1 = *p;
		switch(c){

		case &#39;&quot;&#39;:
		case &#39;\&#39;&#39;:
			for( q=p; *q != c; q++ );
			outnum(q-p);
			ptc(&#39;h&#39;);
			while( p != q )
				ptc(*p++);
			p++;
			break;
		case &#39;&gt;&#39;:
			if( c1==&#39;=&#39; ){
				pts(&quot;.ge.&quot;); p++;
			} else
				pts(&quot;.gt.&quot;);
			break;
		case &#39;&lt;&#39;:
			if( c1==&#39;=&#39; ){
				pts(&quot;.le.&quot;); p++;
			} else if( c1==&#39;&gt;&#39; ){
				pts(&quot;.ne.&quot;); p++;
			} else
				pts(&quot;.lt.&quot;);
			break;
		case &#39;=&#39;:
			if( c1==&#39;=&#39; ){
				pts(&quot;.eq.&quot;); p++;
			} else
				ptc(&#39;=&#39;);
			break;
		case &#39;!&#39;:
			if( c1==&#39;=&#39; ){
				pts(&quot;.ne.&quot;); p++;
			} else
				pts(&quot;.not.&quot;);
			break;
		case &#39;&amp;&#39;:
			if( c1==&#39;&amp;&#39; )
				p++;
			pts(&quot;.and.&quot;);
			break;
		case &#39;|&#39;:
			if( c1==&#39;|&#39; )
				p++;
			pts(&quot;.or.&quot;);
			break;
		case &#39;\t&#39;:
			tabs();
			break;
		case &#39;\n&#39;:
			ptc(&#39; &#39;);
			break;
		default:
			ptc(c);
			break;
		}
	}
}

ptc(c) char c; {
	if( outp &gt; 71 )
		contcard();
	outbuf[outp++] = c;
}

pts(s) char *s; {
	while(*s)
		ptc(*s++);
}

int	contfld	0;

contcard(){
	outbuf[outp] = &#39;\0&#39;;
	printf(&quot;%s\n&quot;, outbuf);
	for( outp=0; outp&lt;contfld-1; outbuf[outp++] = &#39; &#39; );
	outbuf[outp++] = &#39;&amp;&#39;;
}

tabs(){
	ptc(&#39; &#39;);
	while( outp&lt;7 )
		ptc(&#39; &#39;);
	while( outp%3 != 1)
		ptc(&#39; &#39;);
}

outnum(n) int n; {
	int a;
	if( a = n/10 )
		outnum(a);
	ptc(n%10 + &#39;0&#39;);
}

outcont(n) int n; {
	if( n &gt; 0 )
		outnum(n);
	outcode(&quot;\tcontinue&quot;);
	outcode(0);
}

outgoto(n) int n; {
	outcode(&quot;\tgoto &quot;);
	outnum(n);
	outcode(0);
}
