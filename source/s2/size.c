/*
	size -- determine object size

*/

main(argc, argv)
char **argv;
{
	int buf[010], f, ac, sum;

	ac = argc;
	if (ac==1) {
		*argv = &quot;a.out&quot;;
		ac++;
		--argv;
	}
	while(--ac) {
		++argv;
		if((f=open(*argv, 0))&lt;0) {
			printf(&quot;%s not found\n&quot;, *argv);
			continue;
		}
		read(f, buf, 0020);
		if(buf[0]!=0411 &amp;&amp; buf[0]!=0410 &amp;&amp; buf[0]!=0407) {
			printf(&quot;Bad format: %s\n&quot;, *argv);
			close(f);
			continue;
		}
		if (argc&gt;2)
			printf(&quot;%s: &quot;, *argv);
		printf(&quot;%l+%l+%l=&quot;, buf[1],buf[2],buf[3]);
		sum = buf[1]+buf[2]+buf[3];
		printf(&quot;%l (%o)\n&quot;, sum, sum);
		close(f);
	}
}
