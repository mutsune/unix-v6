#

#include &quot;m6.h&quot;

control() {

	while(1) {
		get();
		if(c==0 &amp;&amp; lg&gt;0)  {
			popget(); 
			trash(); 
			rescan = 1; 
		}
		else {
			if(!rescan) put();
			else if(lq&gt;0) {
				if(c==lquote) {
					lq++; 
					put(); 
				}
				else if(c==rquote || c==0)  {
					lq--;
					if(lq&gt;0 || l9&gt;0) put (rquote); 
				}
				else put(); 
			}
			else if(l9&gt;0) 
				if(c==colon||c==0||c==semi) 
					if(--l9&lt;=0) endcall();
					else put();
				else {
					if(c==sharp) l9++;
					else if(c==lquote) lq++;
					put(); 
				}
			else {
				if(c==sharp) begincall();
				else if(c==lquote) lq++;
				else if(lp&gt;0) {
					if(c==colon||c==0||c==semi) endcall();
					else if(c==comma) newarg();
					else put(); 
				}
				else if(c==0) return;	/* lg=lp=lq=0 */
				else put(); 
			} 
		} 
	} 
}

endcall() {
	char *pt, *p;
	rescan = c!=semi;
	newarg();
	pushget();
	pt = &amp;pf-&gt;pa0;
	ge = move(pt,&amp;gf-&gt;ga0,pe-pt);
	if(ge&gt;gmax) diag(&quot;Call stack overflow&quot;);
	if(traceflag) dotrace();
	p = finddef(0);
	setscan(p);
	popput();
	lp--;
	function(p-&gt;dswitch); 
}

begincall() {
	lp++;
	pushput();
	pe = pf+pend;
	pf-&gt;pan = pf-&gt;pa0 = 0;
}

newarg() {
	char *p;
	if(++pf-&gt;pan&gt;=9) if(c==comma) l9++;
	*pe++ = 0;
	pe = (pe+1)&amp;0177776;
	p = &amp;pf-&gt;pa0;
	while(p-&gt;word!=0) p =+ p-&gt;word;
	p-&gt;word = pe - p;
	*pe++ = *pe++ = 0; 
}

setscan(p) 
char *p;
{
	gf-&gt;mchar = (gf-&gt;mframe = p)-&gt;dtext;
	gf-&gt;marg = 0; 
}
