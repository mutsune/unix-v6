# define bufflen 100
IEH3revp (c)
char c;
{
/* reversed line IEH3outputter */
extern char *IEH3outp, *IEH3outlim;
*IEH3outp++ = c;
if (IEH3outp &gt; IEH3outlim+100)
	IEH3err(&quot;unprint producing too many chars&quot;);
}
