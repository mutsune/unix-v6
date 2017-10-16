#

#include &quot;m6.h&quot;

define() {
	remove(1);
	newdef(decbin(3));
	setdef(arg(1),arg(2)); 
}

newdef(n) {
	pushdef();
	de = &amp;df[df-&gt;dtext = dend]; 
	if((df-&gt;dswitch = n)&lt;0) trashflag++;
}

setdef(s1,s2) {
	copydef(s1);
	df-&gt;dtext = de -df;
	copydef(s2); 
}

copydef(s) 
char *s; 
{
	while(*de++ = *s++)
		if(de&gt;dmax) diag(&quot;Definition table overflow&quot;); 
}

char *
arg(n) {
	char *p;
	p = &amp;gf-&gt;ga0;
	while(--n&gt;=0 &amp;&amp; p-&gt;word!=0) p =+ p-&gt;word;
	return(p-&gt;word!=0?p+2:p); 
}

function(n) {
	if(n==0) ;
	else if(1&lt;=n &amp;&amp; n&lt;=13) binop(n);
	else {
		switch(n) {
		case 20: 
			doif(); 
			return;
		case 21: 
			define(); 
			return;
		case 22: 
			copy(); 
			return;
		case 23: 
			meta(); 
			return;
		case 24: 
			size(); 
			return;
		case 25: 
			substr(); 
			return;
		case 26: 
		case 27: 
			go(n); 
			return;
		case 28: 
			remove(1); 
			return;
		case 29: 
			dnl(); 
			return;
		case 32: 
			quote(); 
			return;
		case 33: 
			result(finddef(1)-&gt;dswitch); 
			return;
		case 34: 
			list(); 
			return;
		case 35: 
			traceflag = comp(arg(1),one); 
			return;
		}
	} 
}
