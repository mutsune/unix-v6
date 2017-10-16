#

#include &quot;m6.h&quot;

pushget() {
	ge = (ge+1)&amp;0177776;
	ge-&gt;word = gf-ge; 
	gf = ge; 
	ge =+2;
	++lg;
}

dotrace() {
	char *arg();
	int i,j;
	printf(&quot;\n%d %c%s&quot;,lg,sharp,arg(0));
	for(j=9;j&gt;0&amp;&amp;*arg(j)==0;j--);
	for(i=1;i&lt;=j;i++)
		printf(&quot;%c%c%s%c&quot;,comma,lquote,arg(i),rquote);
	printf(&quot;%c\n&quot;,c); 
}

popget() {
	ge = gf; 
	gf =+ gf-&gt;word;
	--lg;
	if(gf&lt;g0) diag(&quot;Software error&quot;); 
}

pushput() {
	if(pe&amp;1) {
		pf-&gt;prev =| 1;
		pe++; 
	}
	pe-&gt;word = pf-pe; 
	pf = pe; 
	pe =+2;
}

popput() {
	pe = pf; 
	pf =+ pf-&gt;word;
	if(pf-&gt;prev&amp;1) {
		pe--;
		pf-&gt;prev =&amp; 0177776; 
	}
	if(pf&lt;p0) diag(&quot;Software error&quot;); 
}

pushdef() {
	de = (de+1)&amp;0177776;
	de-&gt;word = df-de; 
	df = de; 
	de =+2;
}

put() {
	if(lp&gt;0) {
		*pe++ = c; 
		if(pe&gt;pmax) diag(&quot;Arg collection overflow&quot;); 
	}
	else putchar(c); 
}

get() {
	char *p;
	int n;
	if(lg==0) 
		c = getchar();
	else while(1) {
		if(gf-&gt;marg!=0) {
			if((c = gf[gf-&gt;marg++])==0) gf-&gt;marg = 0;
			else return; 
		}
		c = (p = gf-&gt;mframe)[gf-&gt;mchar++];
		if(c!=dollar) return;
		n = p[gf-&gt;mchar] - &#39;0&#39;;
		if(n&lt;0 || n&gt;9) return;
		++gf-&gt;mchar;
		gf-&gt;marg = arg(n) - gf; 
	}
}
