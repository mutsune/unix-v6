# include &quot;ciodec.c&quot;
cclose (fn)
int fn;
{
struct fileps *fp;
extern char *IEH3olbf[10];
if (fn &lt; 0 || fn &gt; 20) return;
fp = &amp;IEH3fpts[fn];
if (fp-&gt;nchars &gt; 0 || fp-&gt;wrflag == 3)
	switch (fp-&gt;wrflag)
		{
		case 3: seek(fn,0,2);
		case 2: write (fn, fp-&gt;buff, fp-&gt;nchars);
			break;
		case 1: seek(fn, -(fp-&gt;nchars), 1);
		}
fp -&gt; nchars = fp-&gt;eoferr = 0;
fp -&gt; wrflag = 0;
if ( IEH3olbf[fn] != 0 &amp;&amp; fp-&gt;bsize &gt;4)
	free(IEH3olbf[fn]);
IEH3olbf[fn] = 0;
close (fn);
}
char *IEH3olbf[10] {0,0,0,0,0,0,0,0,0,0};
