# include &quot;ciodec.c&quot;
char cgetc(fn)
{
struct fileps *fp;
if (nargs() != 1)
	IEHzap(&quot;cgetc  &quot;);
if (fn &lt;0 || fn &gt;15)
	IEH3err(&quot;cgetc: %d illegal file number&quot;,fn);
fp = &amp;IEH3fpts[fn];
if (fp == 0 || fp-&gt;wrflag &gt; 1)
	IEH3err(&quot;cgetc: %d not open to read&quot;,fn);
if (fp-&gt;wrflag == 0)
	/* file not opened with fopen: try making a buffer */
	IEH3mbuf (fn, 0);
if (fp-&gt;eoferr &gt; 1)
	return (&#39;\0&#39;);
if (fp-&gt;nchars == 0)
	 switch (fp-&gt;nchars = read(fn, fp-&gt;bptr=fp-&gt;buff, fp-&gt;bsize))
		{
		case -1: /* error */
			if (fp-&gt;eoferr == 0)
				IEH3err(&quot;cgetc: error on %d&quot;,fn);
			fp-&gt;eoferr = 3;
			return (&#39;\0&#39;);
		case 0: fp-&gt;eoferr = 2;
			return (&#39;\0&#39;);
		}
fp-&gt;nchars--;
return (*(fp-&gt;bptr++));
}
cin 0;
