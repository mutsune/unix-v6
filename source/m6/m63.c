#

#include &quot;m6.h&quot;

char *
finddef(n) {
	char *p;
	if((p = lookup(arg(n)))==0)
		if((p = lookup(&quot;&quot;))==0) diag(&quot;Software error&quot;);
	return(p); 
}

char *
lookup(s) {
	char *dt;
	dt = df;
	while(dt&gt;d0) {
		if(dt-&gt;dswitch&gt;=0 &amp;&amp; comp(s,&amp;dt-&gt;dident)) return(dt);
		dt =+ dt-&gt;prev; 
	}
	return(0); 
}

comp(s,t) 
char *s, *t; 
{
	for(;*s++ == *t;t++)
		if(*t==0) return(1);
	return(0); 
}

remove(n) {
	char *p;
	if(p = lookup(arg(n))) if(p&gt;d0+2) {
		trashflag++;
		p-&gt;dswitch = -1; 
	} 
}

trash() {
	char *p,*q,*r;
	if(lg&gt;0) return;
	while(df-&gt;dswitch&lt;0) {
		de = df;
		df =+ df-&gt;prev;
		trashflag--; 
	}
	if(trashflag&lt;=0) return;
	de = (de+1)&amp;0177776;
	revptr(de,df,&amp;p,&amp;q);
	q = p;
	for(;p-&gt;word!=0;p=+r-&gt;word) {
		r = p;
		if(p-&gt;dswitch&gt;=0) {
			r = q;
			q = move(p,q,p-&gt;word); 
		} 
	}
	q-&gt;word = 0;
	revptr(d0,d0+2,&amp;df,&amp;de); 
}


revptr(p,q,np,nq) 
char *p, *q, **np, **nq;
{
	int t;
	p-&gt;word = 0;
	while((t = q-&gt;word)!=0) {
		q-&gt;word = p-q; 
		p = q; 
		q =+ t;
		if(q&lt;d0 || q&gt;dmax) diag(&quot;Software error&quot;); 
	}
	*np = p;
	*nq = q; 
}

char *
move(from,to,count) 
char *from, *to;
{
	while(count--&gt;0) *to++ = *from++;
	return(to); 
}
