IEH3err (message, a, b, c, d, e)
char message[];
	{
	extern int cgoof;
	printf(&quot;ERROR &quot;);
	printf(message, a, b, c, d, e);
	cputc(&#39;\n&#39;);
	cexit(cgoof);
	}
cgoof 127;
