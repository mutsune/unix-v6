#

#include &quot;m6.h&quot;

main(argc,argv)
char **argv; 
{
	extern fin;
	int t;
	gf = ge = g0;
	pf = pe = p0;
	t = g0; 
	gmax =+ t-10;
	t = d0; 
	dmax =+ t-10;
	t = p0; 
	pmax =+ t-10;
	revptr(d0,d0+2,&amp;df,&amp;de);
	if(argc&gt;1) {
		fin = open(argv[1],0);
		control();
		close(fin); 
	}
	fin = dup(0);
	control();  
}


diag(m) {
	printf(&quot;%s\n&quot;,m);
	iot(); 
}	/* abort */

int iot 4;

