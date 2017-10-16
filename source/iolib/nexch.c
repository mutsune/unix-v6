IEH3nxch ()
/* returns next character which is not IEH3spce */
{
	char ch, cgetc();
	extern int IEH3sfil;
while ((ch = cgetc(IEH3sfil)) &gt; 0 &amp;&amp; IEH3spce(ch));
if  (ch &gt; 0)
	return (ch);
return (-1);
}
