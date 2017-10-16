#define CHAR	01
#define BLOCK	02
#define INTR	04
#define EVEN	010
#define KL	020
#define ROOT	040
char	*btab[]
{
	&quot;rk&quot;,
	&quot;rp&quot;,
	&quot;rf&quot;,
	&quot;tm&quot;,
	&quot;tc&quot;,
	&quot;hs&quot;,
	&quot;hp&quot;,
	&quot;ht&quot;,
	0
};
char	*ctab[]
{
	&quot;console&quot;,
	&quot;pc&quot;,
	&quot;lp&quot;,
	&quot;dc&quot;,
	&quot;dh&quot;,
	&quot;dp&quot;,
	&quot;dj&quot;,
	&quot;dn&quot;,
	&quot;mem&quot;,
	&quot;rk&quot;,
	&quot;rf&quot;,
	&quot;rp&quot;,
	&quot;tm&quot;,
	&quot;hs&quot;,
	&quot;hp&quot;,
	&quot;ht&quot;,
	0
};
struct tab
{
	char	*name;
	int	count;
	int	address;
	int	key;
	char	*codea;
	char	*codeb;
	char	*codec;
	char	*coded;
	char	*codee;
} table[]
{
	&quot;console&quot;,
	-1,	60,	CHAR+INTR+KL,
	&quot;\tklin; br4\n\tklou; br4\n&quot;,
	&quot;.globl\t_klrint\nklin:\tjsr\tr0,call; _klrint\n&quot;,
	&quot;.globl\t_klxint\nklou:\tjsr\tr0,call; _klxint\n&quot;,
	&quot;&quot;,
	&quot;\t&amp;klopen,   &amp;klclose,  &amp;klread,   &amp;klwrite,  &amp;klsgtty,&quot;,

	&quot;mem&quot;,
	-1,	300,	CHAR,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;\t&amp;nulldev,  &amp;nulldev,  &amp;mmread,   &amp;mmwrite,  &amp;nodev,&quot;,

	&quot;pc&quot;,
	0,	70,	CHAR+INTR,
	&quot;\tpcin; br4\n\tpcou; br4\n&quot;,
	&quot;.globl\t_pcrint\npcin:\tjsr\tr0,call; _pcrint\n&quot;,
	&quot;.globl\t_pcpint\npcou:\tjsr\tr0,call; _pcpint\n&quot;,
	&quot;&quot;,
	&quot;\t&amp;pcopen,   &amp;pcclose,  &amp;pcread,   &amp;pcwrite,  &amp;nodev,&quot;,

	&quot;clock&quot;,
	-2,	100,	INTR,
	&quot;\tkwlp; br6\n&quot;,
	&quot;.globl\t_clock\n&quot;,
	&quot;kwlp:\tjsr\tr0,call; _clock\n&quot;,
	&quot;&quot;,
	&quot;&quot;,

	&quot;parity&quot;,
	-1,	114,	INTR,
	&quot;\ttrap; br7+7.\t\t/ 11/70 parity\n&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,

/*
 * 110 unused
 * 114 memory parity
 * 120 XY plotter
 * 124 DR11-B
 * 130 AD01
 * 134 AFC11
 * 140 AA11
 * 144 AA11
 * 150-174 unused
 */

	&quot;lp&quot;,
	0,	200,	CHAR+INTR,
	&quot;\tlpou; br4\n&quot;,
	&quot;&quot;,
	&quot;.globl\t_lpint\nlpou:\tjsr\tr0,call; _lpint\n&quot;,
	&quot;&quot;,
	&quot;\t&amp;lpopen,   &amp;lpclose,  &amp;nodev,    &amp;lpwrite,  &amp;nodev,&quot;,

	&quot;rf&quot;,
	0,	204,	BLOCK+CHAR+INTR,
	&quot;\trfio; br5\n&quot;,
	&quot;.globl\t_rfintr\n&quot;,
	&quot;rfio:\tjsr\tr0,call; _rfintr\n&quot;,
	&quot;\t&amp;nulldev,\t&amp;nulldev,\t&amp;rfstrategy, \t&amp;rftab,&quot;,
	&quot;\t&amp;nulldev,  &amp;nulldev,  &amp;rfread,   &amp;rfwrite,  &amp;nodev,&quot;,

	&quot;hs&quot;,
	0,	204,	BLOCK+CHAR+INTR,
	&quot;\thsio; br5\n&quot;,
	&quot;.globl\t_hsintr\n&quot;,
	&quot;hsio:\tjsr\tr0,call; _hsintr\n&quot;,
	&quot;\t&amp;nulldev,\t&amp;nulldev,\t&amp;hsstrategy, \t&amp;hstab,&quot;,
	&quot;\t&amp;nulldev,  &amp;nulldev,  &amp;hsread,   &amp;hswrite,  &amp;nodev,&quot;,

/*
 * 210 RC
 */

	&quot;tc&quot;,
	0,	214,	BLOCK+INTR,
	&quot;\ttcio; br6\n&quot;,
	&quot;.globl\t_tcintr\n&quot;,
	&quot;tcio:\tjsr\tr0,call; _tcintr\n&quot;,
	&quot;\t&amp;nulldev,\t&amp;tcclose,\t&amp;tcstrategy, \t&amp;tctab,&quot;,
	&quot;&quot;,

	&quot;rk&quot;,
	0,	220,	BLOCK+CHAR+INTR,
	&quot;\trkio; br5\n&quot;,
	&quot;.globl\t_rkintr\n&quot;,
	&quot;rkio:\tjsr\tr0,call; _rkintr\n&quot;,
	&quot;\t&amp;nulldev,\t&amp;nulldev,\t&amp;rkstrategy, \t&amp;rktab,&quot;,
	&quot;\t&amp;nulldev,  &amp;nulldev,  &amp;rkread,   &amp;rkwrite,  &amp;nodev,&quot;,

	&quot;tm&quot;,
	0,	224,	BLOCK+CHAR+INTR,
	&quot;\ttmio; br5\n&quot;,
	&quot;.globl\t_tmintr\n&quot;,
	&quot;tmio:\tjsr\tr0,call; _tmintr\n&quot;,
	&quot;\t&amp;tmopen,\t&amp;tmclose,\t&amp;tmstrategy, \t&amp;tmtab,&quot;,
	&quot;\t&amp;tmopen,   &amp;tmclose,  &amp;tmread,   &amp;tmwrite,  &amp;nodev,&quot;,

	&quot;ht&quot;,
	0,	224,	BLOCK+CHAR+INTR,
	&quot;\thtio; br5\n&quot;,
	&quot;.globl\t_htintr\n&quot;,
	&quot;htio:\tjsr\tr0,call; _htintr\n&quot;,
	&quot;\t&amp;htopen,\t&amp;htclose,\t&amp;htstrategy, \t&amp;httab,&quot;,
	&quot;\t&amp;htopen,   &amp;htclose,  &amp;htread,   &amp;htwrite,  &amp;nodev,&quot;,

	&quot;cr&quot;,
	0,	230,	CHAR+INTR,
	&quot;\tcrin; br6\n&quot;,
	&quot;&quot;,
	&quot;.globl\t_crint\ncrin:\tjsr\tr0,call; _crint\n&quot;,
	&quot;&quot;,
	&quot;\t&amp;cropen,   &amp;crclose,  &amp;crread,   &amp;nodev,    &amp;nodev,&quot;,

/*
 * 234 UDC11
 */

	&quot;rp&quot;,
	0,	254,	BLOCK+CHAR+INTR,
	&quot;\trpio; br5\n&quot;,
	&quot;.globl\t_rpintr\n&quot;,
	&quot;rpio:\tjsr\tr0,call; _rpintr\n&quot;,
	&quot;\t&amp;nulldev,\t&amp;nulldev,\t&amp;rpstrategy, \t&amp;rptab,&quot;,
	&quot;\t&amp;nulldev,  &amp;nulldev,  &amp;rpread,   &amp;rpwrite,  &amp;nodev,&quot;,

	&quot;hp&quot;,
	0,	254,	BLOCK+CHAR+INTR,
	&quot;\thpio; br5\n&quot;,
	&quot;.globl\t_hpintr\n&quot;,
	&quot;hpio:\tjsr\tr0,call; _hpintr\n&quot;,
	&quot;\t&amp;hpopen,\t&amp;nulldev,\t&amp;hpstrategy, \t&amp;hptab,&quot;,
	&quot;\t&amp;hpopen,   &amp;nulldev,  &amp;hpread,   &amp;hpwrite,  &amp;nodev,&quot;,

/*
 * 260 TA11
 * 264-274 unused
 */

	&quot;dc&quot;,
	0,	308,	CHAR+INTR,
	&quot;\tdcin; br5+%d.\n\tdcou; br5+%d.\n&quot;,
	&quot;.globl\t_dcrint\ndcin:\tjsr\tr0,call; _dcrint\n&quot;,
	&quot;.globl\t_dcxint\ndcou:\tjsr\tr0,call; _dcxint\n&quot;,
	&quot;&quot;,
	&quot;\t&amp;dcopen,   &amp;dcclose,  &amp;dcread,   &amp;dcwrite,  &amp;dcsgtty,&quot;,

	&quot;kl&quot;,
	0,	308,	INTR+KL,
	&quot;\tklin; br4+%d.\n\tklou; br4+%d.\n&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,

	&quot;dp&quot;,
	0,	308,	CHAR+INTR,
	&quot;\tdpin; br6+%d.\n\tdpou; br6+%d.\n&quot;,
	&quot;.globl\t_dprint\ndpin:\tjsr\tr0,call; _dprint\n&quot;,
	&quot;.globl\t_dpxint\ndpou:\tjsr\tr0,call; _dpxint\n&quot;,
	&quot;&quot;,
	&quot;\t&amp;dpopen,   &amp;dpclose,  &amp;dpread,   &amp;dpwrite,  &amp;nodev,&quot;,

/*
 * DM11-A
 */

	&quot;dn&quot;,
	0,	304,	CHAR+INTR,
	&quot;\tdnou; br5+%d.\n&quot;,
	&quot;&quot;,
	&quot;.globl\t_dnint\ndnou:\tjsr\tr0,call; _dnint\n&quot;,
	&quot;&quot;,
	&quot;\t&amp;dnopen,   &amp;dnclose,  &amp;nodev,    &amp;dnwrite,  &amp;nodev,&quot;,

	&quot;dhdm&quot;,
	0,	304,	INTR,
	&quot;\tdmin; br4+%d.\n&quot;,
	&quot;&quot;,
	&quot;.globl\t_dmint\ndmin:\tjsr\tr0,call; _dmint\n&quot;,
	&quot;&quot;,
	&quot;&quot;,

/*
 * DR11-A+
 * DR11-C+
 * PA611+
 * PA611+
 * DT11+
 * DX11+
 */

	&quot;dl&quot;,
	0,	308,	INTR+KL,
	&quot;\tklin; br4+%d.\n\tklou; br4+%d.\n&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,
	&quot;&quot;,

/*
 * DJ11
 */

	&quot;dh&quot;,
	0,	308,	CHAR+INTR+EVEN,
	&quot;\tdhin; br5+%d.\n\tdhou; br5+%d.\n&quot;,
	&quot;.globl\t_dhrint\ndhin:\tjsr\tr0,call; _dhrint\n&quot;,
	&quot;.globl\t_dhxint\ndhou:\tjsr\tr0,call; _dhxint\n&quot;,
	&quot;&quot;,
	&quot;\t&amp;dhopen,   &amp;dhclose,  &amp;dhread,   &amp;dhwrite,  &amp;dhsgtty,&quot;,

/*
 * GT40
 * LPS+
 * VT20
 */

	0
};

char	*stra[]
{
	&quot;/ low core&quot;,
	&quot;&quot;,
	&quot;br4 = 200&quot;,
	&quot;br5 = 240&quot;,
	&quot;br6 = 300&quot;,
	&quot;br7 = 340&quot;,
	&quot;&quot;,
	&quot;. = 0^.&quot;,
	&quot;\tbr\t1f&quot;,
	&quot;\t4&quot;,
	&quot;&quot;,
	&quot;/ trap vectors&quot;,
	&quot;\ttrap; br7+0.\t\t/ bus error&quot;,
	&quot;\ttrap; br7+1.\t\t/ illegal instruction&quot;,
	&quot;\ttrap; br7+2.\t\t/ bpt-trace trap&quot;,
	&quot;\ttrap; br7+3.\t\t/ iot trap&quot;,
	&quot;\ttrap; br7+4.\t\t/ power fail&quot;,
	&quot;\ttrap; br7+5.\t\t/ emulator trap&quot;,
	&quot;\ttrap; br7+6.\t\t/ system entry&quot;,
	&quot;&quot;,
	&quot;. = 40^.&quot;,
	&quot;.globl\tstart, dump&quot;,
	&quot;1:\tjmp\tstart&quot;,
	&quot;\tjmp\tdump&quot;,
	&quot;&quot;,
	0,
};

char	*strb[]
{
	&quot;&quot;,
	&quot;. = 240^.&quot;,
	&quot;\ttrap; br7+7.\t\t/ programmed interrupt&quot;,
	&quot;\ttrap; br7+8.\t\t/ floating point&quot;,
	&quot;\ttrap; br7+9.\t\t/ segmentation violation&quot;,
	0
};

char	*strc[]
{
	&quot;&quot;,
	&quot;/ floating vectors&quot;,
	&quot;. = 300^.&quot;,
	0,
};

char	*strd[]
{
	&quot;&quot;,
	&quot;//////////////////////////////////////////////////////&quot;,
	&quot;/\t\tinterface code to C&quot;,
	&quot;//////////////////////////////////////////////////////&quot;,
	&quot;&quot;,
	&quot;.globl\tcall, trap&quot;,
	0
};

char	*stre[]
{
	&quot;/*&quot;,
	&quot; */&quot;,
	&quot;&quot;,
	&quot;int\t(*bdevsw[])()&quot;,
	&quot;{&quot;,
	0,
};

char	*strf[]
{
	&quot;\t0&quot;,
	&quot;};&quot;,
	&quot;&quot;,
	&quot;int\t(*cdevsw[])()&quot;,
	&quot;{&quot;,
	0,
};

char	*strg[]
{
	&quot;\t0&quot;,
	&quot;};&quot;,
	&quot;&quot;,
	&quot;int\trootdev\t{(%d&lt;&lt;8)|0};&quot;,
	&quot;int\tswapdev\t{(%d&lt;&lt;8)|0};&quot;,
	&quot;int\tswplo\t4000;\t/* cannot be zero */&quot;,
	&quot;int\tnswap\t872;&quot;,
	0,
};

int	fout;
int	root	-1;

main()
{
	register struct tab *p;
	register *q;
	int i, n, ev, nkl;
	int flagf, flagb;

	while(input());

/*
 * pass1 -- create interrupt vectors
 */
	nkl = 0;
	flagf = flagb = 1;
	fout = creat(&quot;l.s&quot;, 0666);
	puke(stra);
	for(p=table; p-&gt;name; p++)
	if(p-&gt;count != 0 &amp;&amp; p-&gt;key &amp; INTR) {
		if(p-&gt;address&gt;240 &amp;&amp; flagb) {
			flagb = 0;
			puke(strb);
		}
		if(p-&gt;address &gt;= 300) {
			if(flagf) {
				ev = 0;
				flagf = 0;
				puke(strc);
			}
			if(p-&gt;key &amp; EVEN &amp;&amp; ev &amp; 07) {
				printf(&quot;\t.=.+4\n&quot;);
				ev =+ 4;
			}
			ev =+ p-&gt;address - 300;
		} else
			printf(&quot;\n. = %d^.\n&quot;, p-&gt;address);
		n = p-&gt;count;
		if(n &lt; 0)
			n = -n;
		for(i=0; i&lt;n; i++)
			if(p-&gt;key &amp; KL) {
				printf(p-&gt;codea, nkl, nkl);
				nkl++;
			} else
			printf(p-&gt;codea, i, i);
	}
	if(flagb)
		puke(strb);
	puke(strd);
	for(p=table; p-&gt;name; p++)
	if(p-&gt;count != 0 &amp;&amp; p-&gt;key &amp; INTR)
		printf(&quot;\n%s%s&quot;, p-&gt;codeb, p-&gt;codec);
	flush();
	close(fout);

/*
 * pass 2 -- create configuration table
 */

	fout = creat(&quot;c.c&quot;, 0666);
	puke(stre);
	for(i=0; q=btab[i]; i++) {
		for(p=table; p-&gt;name; p++)
		if(equal(q, p-&gt;name) &amp;&amp;
		   (p-&gt;key&amp;BLOCK) &amp;&amp; p-&gt;count) {
			printf(&quot;%s\t/* %s */\n&quot;, p-&gt;coded, q);
			if(p-&gt;key &amp; ROOT)
				root = i;
			goto newb;
		}
		printf(&quot;\t&amp;nodev,\t\t&amp;nodev,\t\t&amp;nodev,\t\t0,\t/* %s */\n&quot;, q);
	newb:;
	}
	puke(strf);
	for(i=0; q=ctab[i]; i++) {
		for(p=table; p-&gt;name; p++)
		if(equal(q, p-&gt;name) &amp;&amp;
		   (p-&gt;key&amp;CHAR) &amp;&amp; p-&gt;count) {
			printf(&quot;%s\t/* %s */\n&quot;, p-&gt;codee, q);
			goto newc;
		}
		printf(&quot;\t&amp;nodev,    &amp;nodev,    &amp;nodev,    &amp;nodev,    &amp;nodev,\t/* %s */\n&quot;, q);
	newc:;
	}
	puke(strg, root);
	flush();
	close(fout);
	if(root &lt; 0)
		write(2, &quot;no block device given\n&quot;, 22);
}

puke(s, a)
char **s;
{
	char *c;

	while(c = *s++) {
		printf(c, a);
		printf(&quot;\n&quot;);
	}
}

input()
{
	char line[100];
	register char *p;
	register struct tab *q;
	register n;

	p = line;
	while((n=getchar()) != &#39;\n&#39;) {
		if(n == 0)
			return(0);
		if(n == &#39; &#39; || n == &#39;\t&#39;)
			continue;
		*p++ = n;
	}
	*p++ = 0;
	n = 0;
	p = line;
	while(*p&gt;=&#39;0&#39; &amp;&amp; *p&lt;=&#39;9&#39;) {
		n =* 10;
		n =+ *p++ - &#39;0&#39;;
	}
	if(n == 0)
		n = 1;
	if(*p == 0)
		return(1);
	for(q=table; q-&gt;name; q++)
	if(equal(q-&gt;name, p)) {
		if(root &lt; 0 &amp;&amp; (q-&gt;key&amp;BLOCK)) {
			root = 0;
			q-&gt;key =| ROOT;
		}
		if(q-&gt;count &lt; 0) {
			printf(&quot;%s: no more, no less\n&quot;, p);
			return(1);
		}
		q-&gt;count =+ n;
		if(q-&gt;address &lt; 300 &amp;&amp; q-&gt;count &gt; 1) {
			q-&gt;count = 1;
			printf(&quot;%s: only one\n&quot;, p);
		}
		return(1);
	}
	if(equal(p, &quot;done&quot;))
		return(0);
	printf(&quot;%s: cannot find\n&quot;, p);
	return(1);
}

equal(a, b)
char *a, *b;
{

	while(*a++ == *b)
		if(*b++ == 0)
			return(1);
	return(0);
}

getchar()
{
	int c;

	c = 0;
	read(0, &amp;c, 1);
	return(c);
}
