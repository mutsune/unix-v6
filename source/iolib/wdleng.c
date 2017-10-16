wdleng ()
/* returns number of bits in a machine integer */
/* written so kernighan can tell where he is running */
{
int k, leng;
k = leng = 1;
while (k =&lt;&lt; 1)
	leng++;
return (leng);
}
