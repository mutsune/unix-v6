# include &quot;ciodec.c&quot;
cflush (fn)
int fn;
{
struct fileps *fp;
if (nargs() != 1)
	IEHzap(&quot;cflush&quot;);
if (fn &lt; 0 || fn &gt;15) return;
fp = &amp;IEH3fpts[fn];
if (fp-&gt;nchars &gt; 0 &amp;&amp; fp-&gt;wrflag &gt;= 2)
	{
	write (fn, fp-&gt;buff,fp-&gt;nchars);
	fp-&gt;bptr = fp-&gt;buff;
	fp -&gt; nchars = fp-&gt;eoferr = 0;
	}
if (fp-&gt;wrflag == 1)
	seek(fn, -(fp-&gt;nchars),1);
return;
}
