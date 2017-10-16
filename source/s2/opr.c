/*
 *	opr -- off line print dispatcher
 *		chooses spooling routine appropriate 
 *		to destination
 *
 *	last entry in table isdefault
 */

char *code[] {
	&quot;-lp&quot;,	&quot;/lib/lpr&quot;,	/* line printer */
	0
};

main(argc, argv)
char **argv;
{
	int i, j;

	argv[argc] = 0;
	for(i=0; code[i]; i=+2)
	if(argc &gt; 1)
		for(j=0; code[i][j]==argv[1][j]; j++)
			if(code[i][j] == 0)
					execv(code[i+1], &amp;argv[1]);
	execv(code[i-1], argv);
	write(2, &quot;can&#39;t start daemon\n&quot;, 19);
}
