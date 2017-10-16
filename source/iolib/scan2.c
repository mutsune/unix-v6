_Iint (iptr, length, numbase)
	int *iptr, length;
{
	int n, minus, numdig;
	extern int _Isfil, (*_Iungc)(), (*_Igetc)();
	int c, dval;

n = minus = numdig = 0;
switch ((c=_Inxch()))
	{
	case &#39;-&#39;: minus = 1;
	case &#39;+&#39;: break;
	default: (*_Iungc)(c,_Isfil);
	}
while ((dval=_Idigt(c=((*_Igetc)(_Isfil)), numbase ) ) &gt;= 0 &amp;&amp; numdig++ &lt; length)
	n = n*numbase + dval;
(*_Iungc)(c,_Isfil);
if (numdig == 0)
	return (-1);
if (iptr == 0)
	return (0);
*iptr = minus ? -n : n;
return (1);
}

_Idigt (x, base)
{
switch (x)
	{
	case &#39;0&#39;:
	case &#39;1&#39;:
	case &#39;2&#39;:
	case &#39;3&#39;:
	case &#39;4&#39;:
	case &#39;5&#39;:
	case &#39;6&#39;:
	case &#39;7&#39;:
		return(x-&#39;0&#39;);
	case &#39;8&#39;:
	case &#39;9&#39;:
		if (base &gt; 8)
			return(x - &#39;0&#39;);
	case &#39;a&#39;:
	case &#39;b&#39;:
	case &#39;c&#39;:
	case &#39;d&#39;:
	case &#39;e&#39;:
	case &#39;f&#39;:
		if (base &gt;10)
			return(x - &#39;a&#39; + 10);
	case &#39;A&#39;:
	case &#39;B&#39;:
	case &#39;C&#39;:
	case &#39;D&#39;:
	case &#39;E&#39;:
	case &#39;F&#39;:
		if (base &gt; 10)
			return(x-&#39;A&#39; + 10);
	}
return(-1);
}

_Ilong (dptr, length)
	double *dptr;
	int length;
{
	char temp[75];
	int _Inodg();
	double x;
	double atof();

if (_Isstr(temp, length, _Inodg) &lt; 0)
	return (-1);
x = atof(temp);
if (dptr == 0)
	return (0);
*dptr = x;
return (1);
}

_Isstr (sptr, length, stopf)
	char *sptr;
	int length, (*stopf)();
{
	int ch, initlen, _Inxch();
	extern int _Isfil, (*_Igetc)(), (*_Iungc)();

initlen = length;
if ((ch=_Inxch()) &lt; 0)
	return (-1);
(*_Iungc)(ch,_Isfil);
while (!((*stopf)(ch=(*_Igetc)(_Isfil))) &amp;&amp; length-- &gt; 0)
	if (sptr != 0)
		*(sptr++) = ch;
if (ch &gt;= 0)
	(*_Iungc)(ch,_Isfil);
if (length == initlen)
	return (-1);
if (sptr == 0)
	return (0);
*sptr = &#39;\0&#39;;
return (1);
}

_Iestr (c)
char c;
{
if (_Ispce(c)) return (1);
if (c == &#39;\0&#39;) return (1);
return (0);
}
