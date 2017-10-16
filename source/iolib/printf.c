char  *_ptrbf, *_ptrst, *__fmt;
printf(a1,a2,a3,a4){
auto char  c, *s,  adj, *ptr,*p, buf[30];
extern cputc(),_putstr(), cout;
auto int  *adx, x, n, m, width, prec,i, padchar, fd;
double zz, *dblptr;
char (*f)();
_ptrbf = buf;

fd=cout;
adx = &amp;a1;
f = cputc;
if (a1 == -1)
  {
  f = _putstr;
  _ptrst = a2;
  adx =+ 2;
  }
else if (a1 &gt;= 0 &amp;&amp; a1 &lt;= 9)
  fd = *adx++;
__fmt = *adx++;


while( c = *__fmt++ ){
   if(c != &#39;%&#39;) (*f)(c,fd);
   else { x = *adx++;
      if( *__fmt == &#39;-&#39; ){ adj = &#39;l&#39;;  __fmt++; }
      else adj = &#39;r&#39;;
   padchar = (*__fmt==&#39;0&#39;) ? &#39;0&#39; : &#39; &#39;;
      width = __conv();
      if( *__fmt == &#39;.&#39;){++__fmt; prec = __conv();}
      else prec = 0;

   s = 0;
   switch ( c = *__fmt++ ) {
     case &#39;D&#39;:
     case &#39;d&#39;:
	_prt1(x); break;
     case &#39;o&#39;:
     case &#39;O&#39;:
         _prnt8(x); break;
     case &#39;x&#39;:
     case &#39;X&#39;:
          _prntx(x); break;
      case &#39;S&#39;:
     case &#39;s&#39;:    s=x;
        break;
     case &#39;C&#39;:
     case &#39;c&#39;:   *_ptrbf++ = x&amp;0777;
         break;
     case &#39;E&#39;:
     case &#39;e&#39;:
     case &#39;F&#39;:
     case &#39;f&#39;:
      dblptr = adx-1;
      zz = *dblptr;
      adx =+ 3;
      ftoa (zz, buf, prec, c);
      prec = 0;
      s = buf;
     break;
     default:   (*f)(c,fd);
         adx--;
   }
   if (s == 0)
    {*_ptrbf = &#39;\0&#39;; s = buf;}
   n = _clenf (s);
   n = (prec&lt;n &amp;&amp; prec != 0) ? prec : n;
   m = width-n;
   if (adj == &#39;r&#39;) while (m-- &gt; 0) (*f)(padchar,fd);
   while (n--) (*f)(*s++,fd);
   while (m-- &gt; 0) (*f)(padchar,fd);
   _ptrbf = buf;
   }
}
if(a1 == -1) (*f)(&#39;\0&#39;,fd);
}


_prnt8 (n)
{ /* print in octal */
int p, k, sw;
if (n==0) {*_ptrbf++ = &#39;0&#39;; return;}
sw = 0;
for (p=15; p &gt;= 0; p =- 3)
  if ((k = (n&gt;&gt;p)&amp;07) || sw)
   {
    *_ptrbf++ = &#39;0&#39; + k;
     sw = 1;
     }
}
_prntx (n)
{
	int d,a;
	if (a = n&gt;&gt;4)
		_prntx ( a &amp; 07777);
	d = n&amp;017;
	*_ptrbf++ =  d &gt; 9 ? &#39;A&#39;+d-10 : &#39;0&#39; + d;
}

__conv()
{
auto c,n;
n = 0;
while( ((c = *__fmt++) &gt;= &#39;0&#39;) &amp;&amp; (c&lt;=&#39;9&#39;)) n = n*10+c-&#39;0&#39;;
__fmt--;
return(n);
}

_putstr(chr,str){
*_ptrst++ = chr;
return; ieh305i(); /* force loading of dummy.s */
}
_prt1(n)
{
int digs[15], *dpt;
dpt = digs;
if (n &gt;= 0)
   n = -n;
else
   *_ptrbf++ = &#39;-&#39;;
for (; n != 0; n = n/10)
 *dpt++ = n%10;
if (dpt == digs)
   *dpt++ = 0;
while (dpt != digs)
   { --dpt;
   *_ptrbf++ =  &#39;0&#39; - *dpt;
}
}
