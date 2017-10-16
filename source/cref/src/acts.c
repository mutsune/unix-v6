#
# include &quot;../mcons.h&quot;
# include &quot;../ccmn.h&quot;
# define SKIP 0
# define COLLECT 1
# define SKIP2 2

int	cross	1;
int	order	1;
char	mone	-1;

coll()
{
	cs = COLLECT;
	temp[t1].beg = &amp;line[l];
	return;
}

save()
{
	extern	wspace();

	line[l] = &#39;\0&#39;;
	temp[t1].ct = &amp;line[l] - temp[t1].beg;
	temp[t1].term = c;

	if((c == &#39; &#39; || c == &#39;\t&#39;) &amp;&amp; cflag) {
		gch[++fl] = mone;
		flag[fl] = &amp;wspace;
	} else {
		sav1();
	}
	return;
}

sav1()
{
	extern	only;
	struct tempent	*ptr;
	int	a,tt,val;

	if(cflag &amp;&amp; c == &#39;(&#39; &amp;&amp; level == 0)	csym();

	cs = SKIP;

	ptr = &amp;temp[t1];
	val = search(ptr-&gt;beg,ptr-&gt;ct,&amp;itab,0);

	if(xsw) {
	    switch(val) {
		case 0:
			if((!level&amp;&amp;!hlevel)||(c == &#39;(&#39;)||xtrn
				|| ssw) {
				search(ptr-&gt;beg,ptr-&gt;ct,&amp;xtab,1);
				goto yes;
			} else {
				if(search(ptr-&gt;beg,ptr-&gt;ct,&amp;xtab,0))
					goto yes;
			}
			goto no;

		case 1:
			goto no;

		case 2:
			xtrn = 1;
			goto no;

		case 3:
			if(hlevel)	type = 1;
			if(!level&amp;&amp;!hlevel)	ssw = 1;
			goto no;

		case 4:
			if(hlevel)	type = 1;
			goto no;
	    }
	}

	if(hlevel &amp;&amp; (val == 4 || val == 3))	type = 1;
	if(!val == !only)	goto yes;
no:
	*(ptr-&gt;beg + ptr-&gt;ct) = ptr-&gt;term;
	return(0);
yes:
	tt = t1;
	while(tt)
		if(comp(ptr-&gt;beg,temp[--tt].beg))	goto no;
	t1++;
	return(1);
}

out()
{
	auto	i,ct;
	if(cs == COLLECT)	save();

	ct = t1;
	while(ct--)
		temp[ct].beg[temp[ct].ct] = temp[ct].term;

	while(t1--) {
/*printf(&quot;t1 = %d  beg = %d  ct = %d\n&quot;,t1,temp[t1].beg,temp[t1].ct); /* DEBUG */

		switch(order) {

			case 1:
				if(utmp)
					i = 0;
				else
					i = dfile(temp[t1].beg);

				if((ct = temp[t1].ct) &gt;= 8) {
					ct = 8;
					*curf = -1;
				} else {
					*curf = &#39;\t&#39;;
				}

				put(i,temp[t1].beg,ct);
				put(i,curf,curfl);
				if(cross)	put(i,curs,cursl);
				conf(lno,4,lbuf);
				put(i,lbuf,5);
				break;

			case 2:
				i = dfile(curf+1);
				put(i,curf+1,curfl-1);
				if(cross)	put(i,curs,cursl);
				else {
					conf(lno,4,lbuf);
					put(i,lbuf,5);
				}
				if((ct = temp[t1].ct) &gt;= 8) {
					put(i,temp[t1].beg,8);
					put(i,&amp;mone,1);
				} else {
					put(i,temp[t1].beg,ct);
					put(i,&quot;\t&quot;,1);
				}
				if(cross) {
					conf(lno,4,lbuf);
					put(i,lbuf,5);
				}
				break;

			case 3:
				i = dfile(curs);
				put(i,curs,cursl);
				if((ct = temp[t1].ct) &gt;= 8) {
					put(i,temp[t1].beg,8);
					*curf = -1;
				} else {
					put(i,temp[t1].beg,ct);
					*curf = &#39;\t&#39;;
				}
				put(i,curf,curfl);
				conf(lno,4,lbuf);
				put(i,lbuf,5);
		}
		put(i,line,l + 1);

	}
	t1 = 0;
	l = -1;
	lno++;
	cs = SKIP;

	return;
}


asym()
{
	int	i;
	char	*p;

	if(cs == COLLECT) {
		if(cross) {
			p = temp[t1].beg;
			cursl = &amp;line[l] - p;
			cursl = cursl&gt;8?8:cursl;
			i = -1;
			while(++i &lt; cursl)
				curs[i] = *p++;
			if(cursl &lt; 8)
				curs[cursl++] = &#39;\t&#39;;
			else
				curs[cursl++] = -1;
		}
		save();
	}
	cs = SKIP;
}

asw()
{
	switch(gch[fl]) {

		case 0:
			if(cs == COLLECT)	save();
			cs = SKIP;
			flag[++fl] = &amp;asw;
			gch[fl] = c;
			return(1);

		case &#39;\&#39;&#39;:
			if(c == &#39;\\&#39;) {
				gch[fl] = c;
				return(1);
			}
			break;

		case &#39;&quot;&#39;:
			gch[fl] = &#39;\&#39;&#39;;

			if(c == &#39;\\&#39;) {
				flag[++fl] = &amp;asw;
				gch[fl] = c;
				return(1);
			}
			return(1);

		case &#39;&lt;&#39;:
			if(c == &#39;\n&#39;)	out();
			if(c == &#39;\\&#39;) {
				flag[++fl] = &amp;asw;
				gch[fl] = c;
				return(1);
			}
			if(c != &#39;&gt;&#39;)	return(1);
			break;

		case &#39;/&#39;:
			if(c != &#39;\n&#39;)	return(1);

		case &#39;\\&#39;: 
			if(c == &#39;\n&#39;)	out();

	}
	fl--;
	return(1);

}

csw()
{
	if(cs == COLLECT)	save();

	switch(gch[fl]) {

		case 0:
			if(c == &#39;*&#39;)
				if(line[l - 1] != &#39;/&#39;)
					return(1);
			gch[++fl] = c;
			flag[fl] = &amp;csw;
			return(1);

		case &#39;*&#39;:
			if(c == &#39;\n&#39;)	out();
			if(c == &#39;/&#39; &amp;&amp; line[l - 1] == &#39;*&#39;)
				break;
			return(1);

		case &#39;\&#39;&#39;:
		case &#39;&quot;&#39;:
			if(c == gch[fl])
				break;
			if(c == &#39;\\&#39;) {
				gch[++fl] = c;
				flag[fl] = &amp;csw;
			}
			return(1);

		case &#39;\\&#39;:
			break;
		}
		fl--;
		return(1);
}

incl()
{
/*	printf(&quot;incl: l = %d hl = %d dl = %d\n&quot;,level,hlevel,dlevel);/*DEBUG*/
	if(cs == COLLECT)	save();
	if(hlevel) {
		hlevel = 0;
		level++;
	} else {
		dlevel++;
	}

	cs = SKIP;
}

decl()
{
/*	printf(&quot;decl: l = %d hl = %d dl = %d\n&quot;,level,hlevel,dlevel);/*DEBUG*/
	if(cs == COLLECT)	save();
	cs = SKIP;
	if(dlevel) {
		dlevel--;
		return;
	}
	if(--level &gt; 0)	return;
	curs[0] = &#39;_&#39;;
	curs[1] = &#39;\t&#39;;
	cursl = 2;
	level = 0;
	return;
}

csym()
{
	int	i;
	char	*p;

/*	printf(&quot;csym: l = %d hl = %d dl = %d\n&quot;,level,hlevel,dlevel);/*DEBUG*/
	p = temp[t1].beg;
	if(cs == COLLECT &amp;&amp; level == 0) {
		if(cross) {
			cursl = temp[t1].ct;
			cursl = cursl&gt;8?8:cursl;
			i = -1;
			while(++i &lt; cursl)
				curs[i] = *p++;
			if(cursl &lt; 8)
				curs[cursl++] = &#39;\t&#39;;
			else
				curs[cursl++] = -1;
		}
		hlevel = 1;
	}
	cs = SKIP;
}

dfile(a)
	char	*a;
{
	if(*a &lt; &#39;c&#39;)	return(0);
	if(*a &lt; &#39;h&#39;)	return(1);
	if(*a &lt; &#39;r&#39;)	return(2);
	return(3);
}


sk2()
{
	cs = SKIP2;
}

sk()
{
	cs = SKIP;
}

tabs()
{
	if(l == 0)	l = -1;
}


search(symbol,length,params,install)
	char	*symbol;
	int	length;
	struct	htab	*params;
	int	install;
{
	char	*sp,*p;
	static	int	curb,*hptr,hsiz,nsym,ssiz;
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
			printf(&quot;Too many symbols.\n&quot;);
			dexit();
		}

		hptr[h] = curb;
		length++;
		if((curb + length) &gt;= ssiz) {
			printf(&quot;Too many characters in symbols.\n&quot;);
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

semi()
{
	if(cs == COLLECT)	save();
	if(only)	return;
	xtrn = 0;
	if(!level) {
		ssw = 0;
		if(!type) {
			hlevel = 0;
			curs[0] = &#39;_&#39;;
			curs[1] = &#39;\t&#39;;
			cursl = 2;
		}
		type = 0;
	}
	cs = SKIP;
}

wspace()
{
	if(c == &#39; &#39; || c == &#39;\t&#39;)
		return(1);
	sav1();
	fl--;
	return(0);
}

