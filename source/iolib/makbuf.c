# include &quot;ciodec.c&quot;
IEH3bsz 512;
IEH3mbuf (fn, type)
int fn, type;
{
struct fileps *fp;
extern char *IEH3olbf[], *alloc();
extern int IEHfbak[10];
int bx[19], size, bloc;
fp = &amp;IEH3fpts[fn];
fp-&gt;eoferr = fp-&gt;nchars = 0;
fp-&gt;wrflag = type+1;
/* decide whether to buffer or not */
if (ttyn(fn) != &#39;x&#39;)
	size = 1;
else
if (fstat(fn,bx) &gt; 0 &amp;&amp; bx[0] == 40 &amp;&amp; type == 0)
	size = 1;
else
	size = IEH3bsz;
for (fp-&gt;buff = 0; size &gt;10 &amp;&amp; fp-&gt;buff == 0; size =/ 4)
		if ((bloc = alloc(size+100)) != -1)
			{
			IEH3olbf[fn] = bloc;
			fp-&gt;buff = fp-&gt;bptr =bloc + 100;
			fp-&gt;bsize = size;
			break;
			}
if (fp-&gt;buff == 0)
	{
	fp-&gt;buff = fp-&gt;bptr = &amp;IEHfbak[fn];
	fp-&gt;bsize = size&gt;1 ? 2 : 1;
	}
}
struct fileps IEH3fpts [10];
int IEHfbak[10];
