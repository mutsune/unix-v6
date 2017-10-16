# define PL 102
# define ESC 033
# define SI 017
# define SO 016
# define LINELN 800
char *page[PL];
char lbuff [LINELN], *line;
main ()
{
int c, i, j, ll, cp, mustwr;
	extern int fin, fout;
	fin = dup(0);
	fout = dup(1);

for (ll=0; ll&lt;PL; ll++)
	page[ll] = 0;

c = 1;
cp = ll = 0;
line = lbuff;
mustwr = PL;

while (c&gt;0)
	switch (c = getchar())
		{
		case &#39;\n&#39;:
			store (ll%PL);
			if (++ll &gt;= mustwr)
				if (page[ll%PL] != 0)
					{
					printf (&quot;%s\n&quot;,page[ll%PL]);
					mustwr++;
					free (page[ll%PL]);
					page[ll%PL]=0;
					}
			fetch (ll%PL);
			cp = 0;
			continue;
		case &#39;\0&#39;: continue;
		case ESC:
			c = getchar();
			if (c == &#39;7&#39;)
				{
				store(ll%PL);
				ll--;
				fetch (ll%PL);
				}
			else
				{
				outc (ESC, &amp;line);
				outc (c, &amp;line );
				}
			continue;
		case &#39;\r&#39;:
			line = lbuff;
			continue;
		case &#39;\t&#39;:
			outc (&#39; &#39;, &amp;line);
			cp = line-lbuff;
			while (cp++%8)
				outc(&#39; &#39;, &amp;line);
			continue;
		default:
			outc(c, &amp;line);
		}
for (i=0; i&lt;PL; i++)
   if (page[(mustwr+i)%PL] != 0)
	printf (&quot;%s\n&quot;,page[(mustwr+i) % PL]);
flush();
}
outc (c, lp)
	char **lp;
{
int j;
j = 0;
while (j &gt;0 || *(*lp) == &#39;\b&#39; || *(*lp) == ESC || **lp == SI || **lp == SO)
	{
	switch (*(*lp))
		{
		case &#39;\b&#39;:
			j++;
			(*lp)++;
			break;
		case &#39;\0&#39;:
			*(*lp)++ = &#39; &#39;;
			j--;
			break;
		case ESC: /* &#39;escape&#39; */
			(*lp) =+ 2;
			break;
		case SI:
		case SO:
			(*lp)++;
			break;
		default:
			(*lp)++;
			j--;
			break;
		}
	}
if (c != &#39; &#39; || *(*lp) == &#39;\0&#39;)
	*(*lp) = c;
(*lp)++;
}
store (ll)
{
if (page[ll] != 0)
	free (page[ll]);
page[ll] = alloc ( leng (lbuff) + 2);
copy (page[ll],lbuff);
}
fetch(ll)
{
int i;
for (i=0; i &lt; LINELN; i++)
 lbuff[i] = &#39;\0&#39;;
copy (line=lbuff, page[ll]);
}
copy (s,t)
	char *s, *t;
{
if (t == 0)
	return (*s=0);
while (*s++ = *t++);
}
leng (s)
	char *s;
{
int l;
for (l=0; s[l]; l++);
return (l);
}
