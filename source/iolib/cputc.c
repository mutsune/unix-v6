# include &quot;ciodec.c&quot;
char cputc(ch, fn)
{
struct fileps *fp;
if (nargs() != 2)
  IEHzap(&quot;cputc  &quot;);
if (fn&lt;0 || fn&gt;20) return(ch);
fp = &amp;IEH3fpts[fn];
if (fp-&gt;wrflag == 1)
	IEH3err(&quot;cputc: %d not open&quot;,fn);
if (fp-&gt;wrflag == 0)
	IEH3mbuf(fn,1);
*(fp-&gt;bptr++) = ch;
if (++(fp-&gt;nchars) &lt; fp-&gt;bsize)
	return(ch);
if (fp-&gt;wrflag == 3) /* append, seek to end */
	{
	seek(fn, 0, 2);
	fp-&gt;wrflag = 2; /* now just write from here on */
	}
if ( write(fn, fp-&gt;bptr=fp-&gt;buff, fp-&gt;nchars) &lt; 0)
	{
	if (fp-&gt;eoferr == 0)
		IEH3err(&quot;cputc: writing %d&quot;,fn);
	fp-&gt;eoferr = 3;
	}
fp-&gt;nchars = 0;
return (ch);
}
cout 1;
