gets (s)
char *s;
{ /* gets (s) - read a string with cgetc and store in s */
char *p;
extern int cin;
if (nargs () == 2)
	IEHzap(&quot;gets  &quot;);
p=s;
while ((*s = cgetc(cin)) != &#39;\n&#39; &amp;&amp; *s != &#39;\0&#39;) s++;
if (*p == &#39;\0&#39;) return (0);
*s = &#39;\0&#39;;
return (p);
}
