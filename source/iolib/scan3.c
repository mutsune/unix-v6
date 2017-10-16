_Ierr (message, a, b, c, d, e)
char message[];
	{
	extern int cgoof;
	printf(&quot;ERROR &quot;);
	printf(message, a, b, c, d, e);
	cputc(&#39;\n&#39;);
	cexit(cgoof);
	}
char _Iendm[128] {0};
_Imtab (formatp)
char **formatp;
{
/* make up special table of string ending characters */
int i, normal;
char ch;
/* normally all characters end string except those listed */
normal = 1;
if (**formatp == &#39;^&#39;)
	{normal = 0; (*formatp)++;}
for (i= 0; i &lt; 128; i++)
	_Iendm[i] = normal;
while ((ch = *((*formatp)++)) != &#39;]&#39;)
	_Iendm[ch] = !_Iendm[ch];

}

_Inxch ()
/* returns next character which is not _Ispce */
{
	extern int _Isfil, (*_Igetc)();
        int ch;
while ((ch = (*_Igetc)(_Isfil)) &gt; 0 &amp;&amp; _Ispce(ch));
if  (ch &gt; 0)
	return (ch);
return (-1);
}

_Ispce (c)
char c;
{
switch (c)
	{
	case &#39; &#39;:
	case &#39;\n&#39;:
	case &#39;\t&#39;: return(1);
	}
return(0);
}

_Ispnd (ch)
char ch;
{
return (_Iendm[ch] &gt; 0);
}

char *_Iinpt;
int (*_Igetc)(), (*_Iungc)();
_Igstr ()
{
extern char *_Iinpt;
return (*_Iinpt++);
}

_Iungs(ch)
{
extern char *_Iinpt;
*--_Iinpt = ch;
}
