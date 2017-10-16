# include &quot;../mcons.h&quot;
struct	{
		int	cl[NUMC];
	}tab[NUMS];
char	state[NUMS][SIZS];
char	class[NUMC][SIZC];
char	act[NUMA][SIZA];
char	def[NUMS][SIZA];
char	temp[SIZA];

char	*st[NUMS];
char	*df[NUMS];
char	*cl[NUMC];
char	*ac[NUMA];

int	t1;
int	t2;


main(argc,argv)	char	**argv;
{

	extern	fin;
	char	fl,nlfl,c,bfl,fo,brk;
	int	cs,ca,cc,i,j,cd;

	if(argc != 3) {
		printf(&quot;Usage: mtab input output\n&quot;);
		exit();
	}

	if((fo = creat(argv[2],0644)) &lt; 0) {
		printf(&quot;Output file.\n&quot;);
		exit();
	}
	if((fin = open(argv[1],0)) &lt; 0) {
		printf(&quot;Input file.\n&quot;);
		exit();
	}



	nlfl = 1;
	i = -1;
	while(brk = rword()) {
		switch (brk) {
			case &#39;\n&#39;:
				if(nlfl) {
					move(temp,state[++i]);
					st[i] = &amp;state[i];
				} else {
					move(temp,def[i]);
					df[i] = &amp;def[i];
					nlfl = 1;
				}
				continue;
			case &#39; &#39;:
				if(nlfl) {
					move(temp,state[++i]);
					st[i] = &amp;state[i];
					nlfl = 0;
				} else {
					error(7);
				}
				continue;
		}
	}


	i = 128;
	while(--i) {
		class[i][0] = i;
		class[i][1] = &#39;\0&#39;;
		cl[i] = &amp;class[i];
	}
	cl[0] = &amp;class[0];


	bfl = nlfl = 0;
	t1 = 0;
	t2 = -1;
	while(c = getchar()) {

		switch(c) {

			default:
				if(t1 &gt;= NUMA)	error(4);
				bfl = nlfl = 0;
				act[t1][++t2&lt;8?t2:7] = c;
				continue;

			case &#39;\n&#39;:
				if(nlfl)	break;
				nlfl = 1;

			case &#39; &#39;:
				if(bfl)	continue;
				bfl = 1;
				act[t1][++t2&lt;8?t2:7] = &#39;\0&#39;;
				ac[t1] = &amp;act[t1];
				t1++;
				t2 = -1;
				continue;
		}
	break;
	}
	if(c == &#39;\0&#39;)	exit();

	i = -1;
	while(++i &lt; NUMS) {
		if(df[i]) {
			cd = find(ac,df[i],NUMA);
			j = -1;
			while(++j &lt; NUMC)
				tab[i].cl[j] = cd;
		}
	}


	fl = 0;
	i = -1;
	while(c = getchar()) {

		switch(c) {

			case &#39;\\&#39;:
				temp[++i] = getchar();
				continue;

			case &#39;\n&#39;:
				if(fl != 1)	continue;

			default:
				temp[++i] = c;
				continue;

			case &#39;/&#39;:
				temp[++i] = &#39;\0&#39;;
				i = -1;
				switch(fl) {
					case 0:
						cs = find(st,temp,NUMS);
						fl = 1;
						continue;

					case 1:
						cc = find(cl,temp,NUMC);
						fl = 2;
						continue;

					default:
						error(1);

				}

			case &#39;;&#39;:

				if(fl != 2)	error(2);

				temp[++i] = &#39;\0&#39;;
				i = -1;
				ca = find(ac,temp,NUMA);

/*printf(&quot;%o %o %o\n&quot;,cs,cc,ca); /*DEBUG*/
				tab[cs].cl[cc] = ca;

				fl = 0;
				continue;


		}
	}

	i = -1;
	while(++i &lt; NUMS)
		write(fo,tab[i].cl,256);

}

error(a)
{

	printf(&quot;Error %d\n&quot;,a);
	exit();

}

find(a,b,c)	char	(*a[])[];
		char	b[];
		int	c;
{
	int	i,j;

/*	printf(&quot;%s\n&quot;,b); /*DEBUG*/
	i = -1;
	while(++i &lt; c) {
/*	printf(&quot;	%s\n&quot;,a[i]); /*DEBUG*/
		j = 0;
/*	printf(&quot;b = %c\ta = %c\n&quot;,b[0],(*a[i])[0]); /*DEBUG*/
		while(b[j] == (*a[i])[j]) {
			if(b[j] == &#39;\0&#39;)	goto found;
			j++;
		}
	}
found:
	return(i);
}

rword() {

	char	c;
	int	ct;

	ct = -1;
	while(c = getchar()) {
		switch(c) {

			default:
				temp[++ct] = c;
				continue;

			case &#39;\n&#39;:
				if(ct == -1)	return(&#39;\0&#39;);
			case &#39; &#39;:
				temp[++ct] = &#39;\0&#39;;
				return(c);
		}
	}
}

move(a,b)	char	*a;
		char	*b;
{
	while((*b++ = *a++) != &#39;\0&#39;);
	return;
}
