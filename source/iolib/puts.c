puts(str)
char *str;
{
auto char *p,c;
auto int f;
extern int cout;
if (nargs() != 1)
   IEHzap(&quot;puts  &quot;);
p = str;
while( (c = *p++)!= &#39;\0&#39;) cputc(c,cout);
cputc(&#39;\n&#39;,cout);
return(str);
}
