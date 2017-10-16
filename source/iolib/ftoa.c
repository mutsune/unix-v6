ftoa (x, str, prec, format)
float x;
char *str;
{
/* converts a floating point number to an ascii string */
/* x is stored into str, which should be at least 30 chars long */
int ie, i, k, ndig, fstyle;
double y;
if (nargs() != 7)
  IEHzap(&quot;ftoa  &quot;);
ndig = ( prec&lt;=0) ? 7 : (prec &gt; 22 ? 23 : prec+1);
if  (format == &#39;f&#39; || format == &#39;F&#39;)
 fstyle = 1;
else
 fstyle = 0;
/* print in e format unless last arg is &#39;f&#39; */
ie = 0;
/* if x negative, write minus and reverse */
if ( x &lt; 0)
  {
  *str++ = &#39;-&#39;;
  x = -x;
  }

/* put x in range 1 &lt;= x &lt; 10 */
if (x &gt; 0.0) while (x &lt; 1.0)
  {
  x =* 10.0;
  ie--;
  }
while (x &gt;= 10.0)
  {
  x = x/10.0;
  ie++;
  }

/* in f format, number of digits is related to size */
if (fstyle) ndig =+ ie;

/* round. x is between 1 and 10 and ndig will be printed to
   right of decimal point so rounding is ... */
for (y = i = 1; i &lt; ndig; i++)
  y = y/10.;
x =+ y/2.;
if (x &gt;= 10.0) {x = 1.0; ie++;} /* repair rounding disasters */
/* now loop.  put out a digit (obtain by multiplying by
  10, truncating, subtracting) until enough digits out */
/* if fstyle, and leading zeros, they go out special */
if (fstyle &amp;&amp; ie&lt;0)
  {
  *str++ = &#39;0&#39;; *str++ = &#39;.&#39;;
  if (ndig &lt; 0) ie = ie-ndig; /* limit zeros if underflow */
  for (i = -1; i &gt; ie; i--)
    *str++ = &#39;0&#39;;
  }
for (i=0; i &lt; ndig; i++)
  {
  k = x;
  *str++ = k + &#39;0&#39;;
  if (i == (fstyle ? ie : 0)) /* where is decimal point */
    *str++ = &#39;.&#39;;
  x =- (y=k);
  x =* 10.0;
  }

/* now, in estyle,  put out exponent if not zero */
if (!fstyle &amp;&amp; ie != 0)
  {
  *str++ = &#39;E&#39;;
  if (ie &lt; 0)
    {
    ie = -ie;
    *str++ = &#39;-&#39;;
    }
  for (k=100; k &gt; ie; k =/10);
  for (; k &gt; 0; k =/10)
       {
       *str++ = ie/k + &#39;0&#39;;
       ie = ie%k;
       }
  }
*str = &#39;\0&#39;;
return;
}
