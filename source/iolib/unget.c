# include &quot;ciodec.c&quot;
ungetc (c, fn)
{
struct fileps *fp;
if (nargs() != 2)
   IEHzap(&quot;ungetc&quot;);
/* push back onto input */
fp = &amp;IEH3fpts[fn];
if (fp-&gt;wrflag == 0)
	IEH3mbuf(fn,0);
if (fp-&gt;bptr &lt;= fp-&gt;buff - 100)
	IEH3err(&quot;ungetc/unprintf: buffer full file %d&quot;,fn);
*--fp-&gt;bptr = c;
fp-&gt;nchars++;
}
