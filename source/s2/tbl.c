#
# define ever (;;)
# define MAXCHS 2000
main(argc,argv)
	char *argv[];
{
char line[200];
extern int cin;
extern char *cspace;
	cspace = getvec(MAXCHS + 100);
for ever
{
	if (argc&gt;1)
	{
	argc--;
	argv++;
	if (match(*argv, &quot;-ms&quot;))
		*argv = &quot;/usr/lib/tmac.s&quot;;
	cin = copen(*argv, &#39;r&#39;);
	if (cin &lt; 0)
		{
		printf(2,&quot;where is input file %s\n&quot;,*argv);
		cexit();
		}
	}
while (gets(line))
	if (match(line, &quot;.TS&quot;))
		tableput();
	else
		puts(line);
if (argc &lt;= 1) break;
}
cexit();
}
# define MAXCOL 35
# define MAXLIN 220
# define ONELINE 250
char *tabentry[MAXLIN][MAXCOL];
char extra[MAXCHS];
char *cspace, *cstore;
char *exstore extra;
int sep[MAXCOL], dwide[MAXCOL];
char *instead[MAXLIN];
int newtab[MAXLIN];
char *style[MAXCOL];
tableput()
{
/* read input, write output, make tables on the way */
char st[ONELINE], *format;
int ilin, nlin, icol, ncol, k, ch, ws, cs, ns;
int maxchnge, ct;
printf(&quot;.TS\n&quot;);
/* get command line */
cstore = cspace;
exstore = extra;
ncol = 0;
for (ilin=0; ilin&lt;MAXLIN; ilin++)
	newtab[ilin]=0;
gets (st);
for (k=0; k&lt;ONELINE &amp;&amp; st[k] != &#39;\0&#39;; k++)
	if (!space(st[k]))
		{
		style[ncol] = st+k;
		dwide[ncol] =0;
		for(; letter(st[k]); k++)
			{
			if ((st[k]==&#39;n&#39; || st[k] == &#39;N&#39;) &amp;&amp;
				!dwide[ncol])
				{
				dwide[ncol]=1;
				sep[ncol++] = 0;
				style[ncol] = style[ncol-1];
				dwide [ncol] = 0;
				}
			}
		if (digit(st[k]))
			sep[ncol] = numb(st+k);
		else
			sep[ncol] = 3;
		ncol++;
		while (digit(st[k]))
			st[k++] = &#39;\0&#39;;
		if (st[k] == &#39;\0&#39;)
			break;
		st[k] = &#39;\0&#39;;
		}
/* now get input lines */
for (nlin=0; gets(cstore) &amp;&amp; !match(cstore, &quot;.TE&quot;); nlin++)
	{
	if (cstore[0] == &#39;.&#39; &amp;&amp; letter(cstore[1]))
		{
		instead[nlin] = cstore;
		while (*cstore) cstore++;
		}
	else instead[nlin] = 0;
	for (icol = 0; icol &lt;ncol; icol++)
		{
		tabentry[nlin][icol] = cstore;
		for(; (ch= *cstore) != &#39;\0&#39; &amp;&amp; ch != &#39;\t&#39;; cstore++)
				;
		*cstore++ = &#39;\0&#39;;
		if (dwide[icol] )
			if (broken(style[icol],nlin))
				{
				tabentry[nlin][icol+1]=maknew(tabentry[nlin][icol]);
				icol++;
				if (tabentry[nlin][icol] ==0)
					newtab[nlin]=newtab[nlin+1]=1;
				}
			else
				tabentry[nlin][++icol] = 0;
		while (span(style[icol+1],nlin) )
			tabentry[nlin][++icol] = &quot;&quot;;
		if (ch == &#39;\0&#39;) break;
		}
	while (++icol &lt;ncol)
		tabentry[nlin][icol] = &quot;&quot;;
	while (*cstore != &#39;\0&#39;)
		 *cstore++;
	if (cstore-cspace &gt; MAXCHS)
		cstore = cspace  = getvec(MAXCHS+100);
	}
/* find longest command string */
for (icol=maxchnge=0; icol&lt;ncol; icol++)
	if (slen(style[icol]) &gt;maxchnge)
		maxchnge = slen(style[icol]);
/* set tab stops */
printf(&quot;.nr 49 0\n&quot;);
for (icol = 0; icol&lt;ncol; icol++)
	{
	ct = 1 + (icol&gt;0 ? sep[icol-1] : 0);
	printf(&quot;.nr %2d 0\n&quot;, icol+50);
	for (ilin=0; ilin &lt; nlin; ilin++)
		{
		if (icol&gt;0 &amp;&amp; dwide[icol-1]&gt;0 &amp;&amp; tabentry[ilin][icol]==0)
			{
			printf(&quot;.nr 48 \\n(%2d+\\w&#39;%s&#39;+%dn\n&quot;,
				icol+48, tabentry[ilin][icol-1], sep[icol-1]);
			printf(&quot;.if \\n(48-\\n(%2d .nr %2d \\n(48\n&quot;,
				icol+50,icol+50);
			}
		if ( !span(style[icol],ilin) &amp;&amp; /* not part of span */
		   (dwide[icol] == 0 || tabentry[ilin][icol+1]!= 0)
				/* not a double item */
		     &amp;&amp; (!span(style[icol+1],ilin) || icol==ncol))
			{
			printf(&quot;.nr 47 \\n(%2d+\\w&#39;%s&#39;+%dn\n&quot;,
			icol+49,tabentry[ilin][icol], ct);
			printf(&quot;.if \\n(47-\\n(%2d .nr %2d \\n(47\n&quot;,
				icol+50,icol+50);
			}
		}
	/* clean up spanned headings */
	for(ilin=0; ilin&lt;maxchnge; ilin++)
		{
		if( !(span(style[icol],ilin)) &amp;&amp;
		  (icol ==1 || dwide[icol-1] == 0) &amp;&amp;
		  span(style[icol+(dwide[icol]?2:1)],ilin))
			printf(&quot;.nr %d \\n(%2d+\\w&#39;%s&#39;+%dn\n&quot;,
			  ilin+30, icol+49, tabentry[ilin][icol], ct);
		else if (span(style[icol],ilin) &amp;&amp;
		  (icol+1==ncol || !span(style[icol+1],ilin)))
			printf(&quot;.if \\n(%d-\\n(%d .nr %d \\n(%d\n&quot;,
				30+ilin, icol+50, icol+50, ilin+30);
		}
	}
/* run out table, put field characters in */
printf (&quot;.fc  @\n&quot;);
for (ilin=0; ilin&lt;nlin; ilin++)
	{
	if (instead[ilin])
		{
		printf(&quot;%s\n&quot;,instead[ilin]);
		continue;
		}
	/* is a new set of tab stops needed */
	if (ilin &lt; maxchnge || newtab[ilin])
		settab(ncol, ilin);
	for (icol=0; icol&lt;ncol; icol++)
		{
		switch ( ylet(style[icol],ilin))
			{
			default:
			case &#39;L&#39;: case &#39;l&#39;:
				format = &quot;%s@&quot;; break;
			case &#39;R&#39;: case &#39;r&#39;:
				format= &quot;@%s&quot;; break;
			case &#39;n&#39;: case &#39;N&#39;:
				if (!dwide[icol] || tabentry[ilin][icol+1] != 0)
					{
					format=dwide[icol]? &quot;@%s&quot; : &quot;%s@&quot;;
					break;
					}
			case &#39;c&#39;: case &#39;C&#39;:
				format = &quot;@%s@&quot;; break;
			case &#39;s&#39;:  case &#39;S&#39;:
				format= &quot;&quot;;
				break;
			}
		if (! (dwide [icol-1]&gt;0 &amp;&amp; tabentry[ilin][icol] == 0 ))
		printf(format, tabentry[ilin][icol]);
		if (!span(style[icol+1],ilin))
			for (k=sep[icol]; k&gt;0; k--)
				printf(&quot; &quot;);
		}
	printf(&quot;\n&quot;);
	}
printf(&quot;.fc\n&quot;);
printf(&quot;.TE\n&quot;);
}
match (s1, s2)
	char *s1, *s2;
{
	while (*s1 == *s2)
		if (*s1++ == &#39;\0&#39;)
			return(1);
		else
			*s2++;
	return(0);
}
slen(str)
	char *str;
{
	int k;
	for (k=0; *str++ != &#39;\0&#39;; k++);
	return(k);
}
space(ch)
	{
	switch (ch)
		{
		case &#39; &#39;: case &#39;\t&#39;:
			return(1);
	}
	return(0);
	}
letter (ch)
	{
	if (ch &gt;= &#39;a&#39; &amp;&amp; ch &lt;= &#39;z&#39;)
		return(1);
	if (ch &gt;= &#39;A&#39; &amp;&amp; ch &lt;= &#39;Z&#39;)
		return(1);
	return(0);
	}
numb(str)
	char *str;
	{
	/* convert to integer */
	int k;
	for (k=0; *str &gt;= &#39;0&#39; &amp;&amp; *str &lt;= &#39;9&#39;; str++)
		k = k*10 + *str - &#39;0&#39;;
	return(k);
	}
broken(str, nlin)
	{
	switch(ylet(str,nlin))
		{
		case &#39;n&#39;: case &#39;N&#39;:
			return(1);
		}
	return(0);
	}
ylet (str, k)
	char *str;
{
	k++;
	while (*str &amp;&amp;k--)
		str++;
	return(*--str);
}
span(str, k)
	{
	switch(ylet(str,k))
		{
		case &#39;s&#39;: case &#39;S&#39;:
			return(1);
		}
	return(0);
	}
maknew(str)
	char *str;
{
	/* make two numerical fields */
	int point;
	char *p, *q;
	p = str;
	for (point=0; *str; str++)
		if (*str==&#39;.&#39;)
			point=1;
	if (!point &amp;&amp; *(str-1)== &#39;$&#39;)
		return(0);
	for(; str&gt;p; str--)
		if ( (point &amp;&amp; *str == &#39;.&#39;) ||
		    (!point &amp;&amp; digit(*(str-1)) ) )
			break;
	if (!point &amp;&amp; p==str) /* not numerical, don&#39;t split */
		return(0);
	p= str;
	q = exstore;
	while (*exstore++ = *str++);
	*p = 0;
	return(q);
	}
digit(x)
	{
	return(x&gt;= &#39;0&#39; &amp;&amp; x&lt;= &#39;9&#39;);
	}
settab(ncol, ilin)
{
	int k, icol;
	printf(&quot;.ta &quot;);
	for (icol = 0; icol&lt;ncol; icol++)
	   if ((dwide[icol] == 0 || tabentry[ilin][icol+1] != 0)
		&amp;&amp; !span(style[icol+1],ilin))
		printf(&quot;\\n(%du &quot;,icol+50);
	printf(&quot;\n&quot;);
}
