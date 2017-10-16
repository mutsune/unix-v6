_clenf (s) /* counts length of string */
char *s;
{
int n;
for (n=0; *s++ != &#39;\0&#39;; n++);
return (n);}
