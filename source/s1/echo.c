main(argc, argv)
int argc;
char *argv[];
{
	int i;

	argc--;
	for(i=1; i&lt;=argc; i++)
		printf(&quot;%s%c&quot;, argv[i], i==argc? &#39;\n&#39;: &#39; &#39;);
}
