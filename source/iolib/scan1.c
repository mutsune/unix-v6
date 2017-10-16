scanf (p1, p2, p3, p4)
	int p1, p2, p3, p4;
{
/* first arg can be a control string, a file id, or -1 */
	int ptrs[10], j, ip, flp, k;
	char *np;
/*	extern int cin;*/
extern (*_Igetc)(), (*_Iungc)(), cgetc(), ungetc(), _Igstr(), _Iungs();
extern char *_Iinpt;
ip = 0;
if (p1 == -1)
  {k = 1; _Iinpt = p2;}
else if (p1 &gt;= 0 &amp;&amp; p1 &lt; 10)
  k = 0;
else
  k = -1;
if (k &lt;= 0)
  {_Igetc = cgetc; _Iungc = ungetc;}
else
  {_Igetc = _Igstr; _Iungc = _Iungs;}
j = 0;
for (np = (&amp;p2)[k]; *np; np++)
    if (*np == &#39;%&#39; &amp;&amp; *(np+1) != &#39;%&#39; &amp;&amp; *(np+1) != &#39;*&#39;)
	ptrs[ip++] = (&amp;p3)[(j++)+k];
return (_Iscan ((k==0 ?  p1 : 0), (&amp;p2)[k], ptrs));
}

_Iscan (fileid, format, listp)
	char *format;
	int *listp;
{
	char ch, _Inxch();
	int nmatch;
	extern int _Isfil;
	_Isfil = fileid;
nmatch = 0;
while (1) switch (ch= *format++)
	{
	case &#39;\0&#39;: return (nmatch);
	case &#39;%&#39;: switch (_Isfrm(&amp;format, *listp++))
			{
			case 0: listp--; break;
			case -1: return (nmatch &gt; 0 ? nmatch : -1);
			default: nmatch++;
			}
	case &#39; &#39;:
	case &#39;\n&#39;:
	case &#39;\t&#39;: break;
	default: if (ch != _Inxch())
			return(nmatch);
	}
}

int _Isfil 0;

_Ichar (cptr)
	char *cptr;
{
	char ch, _Inxch();

if ((ch = _Inxch()) &lt; 0)
	return (-1);
if (cptr == 0)
	return (0);
*cptr = ch;
return (1);
}

_Iflot (fptr, length)
	float *fptr;
	int length;
{
	char temp[75];
	int _Inodg();
	float x;
	double atof();

if (_Isstr(temp, length, _Inodg) &lt; 0)
	return (-1);
x = atof(temp);
if (fptr == 0)
	return (0);
*fptr = x;
return (1);
}

_Inodg (ch)
char ch;
{
if (_Idigt(ch,10) &gt;= 0) return (0);
switch (ch)
	{
	case &#39;E&#39;:
	case &#39;e&#39;:
	case &#39;.&#39;: case &#39;+&#39;: case &#39;-&#39;:
		return (0);
	}
return (1);
}

_Isfrm (spec, pointer)
	char **spec;
	int pointer;
{
	int length, lflag, _Iestr(), _Ispnd();
	char ch;
length = lflag = 0;
while (1) switch (ch = *((*spec)++))
	{
	case &#39;*&#39;: pointer=0; break;
	case &#39;0&#39;: case &#39;1&#39;: case &#39;2&#39;: case &#39;3&#39;: case &#39;4&#39;:
	case &#39;5&#39;: case &#39;6&#39;: case &#39;7&#39;: case &#39;8&#39;: case &#39;9&#39;:
		length = length*10 + ch - &#39;0&#39; ;
		lflag++;
		break;
	case &#39;o&#39;: /* octal */
		return(_Iint(pointer, lflag ? length : 100, 8));
	case &#39;x&#39;: /* hex */
		return(_Iint(pointer, lflag ? length : 100, 16));
	case &#39;d&#39;: /* decimal */
		return (_Iint(pointer, lflag ? length : 100, 10));
	case &#39;c&#39;: /* character */
		return (_Ichar(pointer));
	case &#39;s&#39;: /* string */
		return (_Isstr(pointer, lflag ? length : 100, _Iestr));
	case &#39;f&#39;:
	case &#39;e&#39;: /* float */
		return (_Iflot(pointer, lflag ? length : 100));
	case &#39;l&#39;: /*  (long) double or int */
		switch(*(*spec)++)
			{
			case &#39;f&#39;: case &#39;F&#39;:
			case &#39;e&#39;: case &#39;E&#39;:
				return (_Ilong (pointer, lflag ? length : 100));
			default: printf(2, &quot;long not yet implemented\n&quot;);
				return(0);
			}
	case &#39;[&#39;: /* special strings */
		_Imtab(spec);
		return (_Isstr (pointer, lflag ? length : 100, _Ispnd));
	case &#39;%&#39;:
		if (_Inxch() != &#39;%&#39;)
			return (-1);
		return(0);
	case &#39;\0&#39;:
		_Ierr(&quot;scanf: bad format termination\n&quot;);
	default: _Ierr (&quot;scanf: format character %c&quot;, ch);
	}
}
