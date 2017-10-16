cexit (rcode)
{
/* closes all files and exits */
int i;
for (i = 0; i &lt; 10; i++)
	cclose(i);
exit(rcode);	/* rcode courtesy of sny */
}
