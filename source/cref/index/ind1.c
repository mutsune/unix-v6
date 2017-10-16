#
# include &quot;econs.h&quot;
# include &quot;ecmn.h&quot;
# define SKIP 0
# define COLLECT 1
# define SKIP2 2

char	mone	-1;
int	tlno	1;

coll()
{
	cs = COLLECT;
	temp[t1].beg = &amp;line[l];
	return;
}

save()
{
	extern	only;
	char	*pt1,*pt2,cbuf[30];
	int	a,tt,val;

	cs = SKIP;
	line[l] = &#39;\0&#39;;
	temp[t1].ct = &amp;line[l] - temp[t1].beg;
	if(!count)
		if(temp[t1].ct == 1)	goto no;
	pt1 = temp[t1].beg-1;
	pt2 = cbuf-1;

	while(*++pt2 = *++pt1)
		if(*pt2 &gt;= &#39;A&#39; &amp;&amp; *pt2 &lt;= &#39;Z&#39;)
			*pt2 =| 040;

	if(count)
		goto yes;
	val = search(cbuf,temp[t1].ct,&amp;itab,0);

	if(!val == !only)	goto yes;
no:
	line[l] = c;
	return(0);
yes:
	if(count == 0) {
		tt = t1;
		while(tt)
			if(comp(temp[t1].beg,temp[--tt].beg))	goto no;
	}
	temp[t1++].term = c;
	return(1);
}

out()
{
	auto	i,ct,t2;
	char	*b,*e;
	if(cs == COLLECT)	save();
	ct = t1;
	while(ct--)
		temp[ct].beg[temp[ct].ct] = temp[ct].term;
	t2 = t1 - 1;
	while(t1--) {
/*printf(&quot;t1 = %d  beg = %o  ct = %d\n&quot;,t1,temp[t1].beg,temp[t1].ct); /* DEBUG */
		if((ct = temp[t1].ct) &gt;= 15) {
			ct = 15;
		}
		put(temp[t1].beg,ct);
		if(count) {
			put(&quot;\n&quot;, 1);
			continue;
		}

		put(&quot;\t&quot;,1);

		if(!page) {
			put(curf,curfl);
			conf(lno,4,curs);
			put(curs,4);
		} else {
			conf(pn,4,curs);
			put(curs,4);
		}
		if(word == 0) {
			put(&quot;\t&quot;,1);
			if(t1 &gt;= 1)
				b = temp[t1-1].beg;
			else
				b =  line;
			if(t2 &gt; t1)
				e = temp[t1+1].beg + temp[t1+1].ct;
			else
			e = &amp;line[l];
/*printf(&quot;e = %o	b = %o\n&quot;,e,b);	/*DEBUG*/
			put(b,e-b);
		}
		put(&quot;\n&quot;,1);
	}
	t1 = 0;
	l = -1;
	lno =+ tlno;
	tlno = 1;
	cs = SKIP;
	return;
}


search(symbol,length,params,install)
	char	*symbol;
	int	length;
	struct	htab	*params;
	int	install;
{
	char	*sp,*p;
	static	int	*hptr,hsiz,nsym;
	static	char	*ssiz;
	static	int	curb;
	static	char	*symt;
	auto	h,i,j,k;

	if(hptr != params-&gt;hptr) {
		hptr = params-&gt;hptr;
		hsiz = params-&gt;hsiz;
		symt = params-&gt;symt;
		ssiz = params-&gt;ssiz;
		curb = params-&gt;curb;
		nsym = params-&gt;nsym;
	}

	symbol[length] = &#39;\0&#39;;
/*printf(&quot;ssiz = %d; nsym = %d; %s\n&quot;, ssiz, nsym, symbol);/*DEBUG*/
	sp = symbol;

	i = length;
	h = 1;
	while(i--)
		h =* *sp++;

	if(h == 0100000) {
		h = 1;
	} else {
		h = h&lt;0?(-h)%hsiz:h%hsiz;
	}
	if(h == 0)	h++;
/*		printf(&quot;%s %d\n&quot;,symbol,h);	/*DEBUG*/

	while((p = &amp;symt[hptr[h]]) &gt; symt) {
		j = length + 2;
		sp = symbol;
		while(--j) {
			if(*p++ != *sp++)	goto no;
		}
		return(*p);
no:
		h = (h + h)%hsiz;
	}
	if(install) {
		if(++nsym &gt;= hsiz) {
			printf(&quot;Too many symbols in ignore/only file.\n&quot;);
			dexit();
		}

		hptr[h] = curb;
		length++;
		if((curb + length) &gt;= ssiz) {
			printf(&quot;i/o file too big; ssiz = %d\n&quot;, ssiz);
			dexit();
		}

		while(length--)
			symt[curb++] = *symbol++;
		symt[curb++] = install;
		params-&gt;curb = curb;
		params-&gt;nsym = nsym;
	}
	return(0);
}

conf(n,width,buf) 
	char	*buf;
{
	auto	i,a;

	i = width;
	while(i--)	buf[i] = &#39; &#39;;

	buf[(a = n/10)?conf(a,--width,buf):--width] = n%10 + &#39;0&#39;;

	return(++width);
}

comp(a,b)
	char	*a;
	char	*b;
{
	a--;
	b--;
	while(*++a == *++b)
		if(*a == &#39;\0&#39;)	return(1);
	return(0);
}


hyphen()
{
/*	printf(&quot;hyphen\n&quot;);	/*DEBUG*/
	if(gch[fl] == 0)
		flag[++fl] = &amp;hyp1;
	return(1);
}

hyp1()
{
/*	printf(&quot;hyp1 c = %o\n&quot;,c);	/*DEBUG*/
	if(c !=  &#39;\n&#39;) {
		fl--;
		return(0);
	} else {
		l =- 2;
		flag[fl] = &amp;hyp2;
		hsw = 1;
		return(1);
	}
}

hyp2()
{
/*	printf(&quot;hyp2 c = %o l = %d\n&quot;,c,l);	/*DEBUG*/
	if(hsw &amp;&amp; (tab[2].cl[c] == 0)) {
		l--;
		if(c == 3)	pno();
		if(c == &#39;\n&#39;)	tlno++;
		return(1);
	}
	hsw = 0;
	if(tab[cs].cl[c]) {
		line[l] = &#39;\n&#39;;
		out();
		fl--;
		return(0);
	}
	return(1);
}

pno()
{
	if(flag[fl] != &amp;pno) {
		flag[++fl] = &amp;pno;
		pn = 0;
		return(1);
	}
	if(c == &#39;\n&#39;) {
		fl--;
		out();
		return(1);
	}
	pn = pn*10 + c - &#39;0&#39;;
	return(1);
}
