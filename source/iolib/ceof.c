# include &quot;ciodec.c&quot;
ceof (fn)
int fn;
{
struct fileps *fp;
fp = &amp;IEH3fpts[fn];
if (fp-&gt;wrflag &gt; 0 &amp;&amp; fp-&gt;eoferr == 2)
	return(1);
else return(0);
}
cerr (fn)
int fn;
{
struct fileps *fp;
fp = &amp;IEH3fpts[fn];
if (fp-&gt;eoferr == 0) fp-&gt;eoferr = 1;
if (fp-&gt;wrflag &gt; 0 &amp;&amp; fp-&gt;eoferr == 3)
	return(1);
else return(0);
}
