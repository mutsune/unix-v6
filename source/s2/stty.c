#
/*
 * set teletype modes
 */

/*
 * tty flags
 */
#define	HUPCL	01
#define	XTABS	02
#define	LCASE	04
#define	ECHO	010
#define	CRMOD	020
#define	RAW	040
#define	ODDP	0100
#define	EVENP	0200
#define	ANYP	0300

/*
 * Delay algorithms
 */
#define	CR0	0
#define	CR1	010000
#define	CR2	020000
#define	CR3	030000
#define	NL0	0
#define	NL1	000400
#define	NL2	001000
#define	NL3	001400
#define	TAB0	0
#define	TAB1	002000
#define	TAB2	004000
#define	TAB3	006000
#define	FF0	0
#define	FF1	040000
#define	BS0	0
#define	BS1	0100000
#define	ALL	0177400

struct
{
	char	*string;
	int	speed;
} speeds[]
{
	&quot;0&quot;,	(0&lt;&lt;8)|0,
	&quot;50&quot;,	(1&lt;&lt;8)|1,
	&quot;75&quot;,	(2&lt;&lt;8)|2,
	&quot;110&quot;,	(3&lt;&lt;8)|3,
	&quot;134&quot;,	(4&lt;&lt;8)|4,
	&quot;134.5&quot;,(4&lt;&lt;8)|4,
	&quot;150&quot;,	(5&lt;&lt;8)|5,
	&quot;200&quot;,	(6&lt;&lt;8)|6,
	&quot;300&quot;,	(7&lt;&lt;8)|7,
	&quot;600&quot;,	(8&lt;&lt;8)|8,
	&quot;1200&quot;,	(9&lt;&lt;8)|9,
	&quot;1800&quot;,	(10&lt;&lt;8)|10,
	&quot;2400&quot;,	(11&lt;&lt;8)|11,
	&quot;4800&quot;,	(12&lt;&lt;8)|12,
	&quot;9600&quot;,	(13&lt;&lt;8)|13,
	&quot;exta&quot;,	(14&lt;&lt;8)|14,
	&quot;extb&quot;,	(15&lt;&lt;8)|15,
	0,
};
struct
{
	char	*string;
	int	set;
	int	reset;
} modes[]
{
	&quot;even&quot;,
	EVENP, 0,

	&quot;-even&quot;,
	0, EVENP,

	&quot;odd&quot;,
	ODDP, 0,

	&quot;-odd&quot;,
	0, ODDP,

	&quot;raw&quot;,
	RAW, 0,

	&quot;-raw&quot;,
	0, RAW,

	&quot;cooked&quot;,
	0, RAW,

	&quot;-nl&quot;,
	CRMOD, 0,

	&quot;nl&quot;,
	0, CRMOD,

	&quot;echo&quot;,
	ECHO, 0,

	&quot;-echo&quot;,
	0, ECHO,

	&quot;LCASE&quot;,
	LCASE, 0,

	&quot;lcase&quot;,
	LCASE, 0,

	&quot;-LCASE&quot;,
	0, LCASE,

	&quot;-lcase&quot;,
	0, LCASE,

	&quot;-tabs&quot;,
	XTABS, 0,

	&quot;tabs&quot;,
	0, XTABS,

	&quot;hup&quot;,
	HUPCL, 0,

	&quot;-hup&quot;,
	0, HUPCL,

	&quot;cr0&quot;,
	CR0, CR3,

	&quot;cr1&quot;,
	CR1, CR3,

	&quot;cr2&quot;,
	CR2, CR3,

	&quot;cr3&quot;,
	CR3, CR3,

	&quot;tab0&quot;,
	TAB0, TAB3,

	&quot;tab1&quot;,
	TAB1, TAB3,

	&quot;tab2&quot;,
	TAB2, TAB3,

	&quot;tab3&quot;,
	TAB3, TAB3,

	&quot;nl0&quot;,
	NL0, NL3,

	&quot;nl1&quot;,
	NL1, NL3,

	&quot;nl2&quot;,
	NL2, NL3,

	&quot;nl3&quot;,
	NL3, NL3,

	&quot;ff0&quot;,
	FF0, FF1,

	&quot;ff1&quot;,
	FF1, FF1,

	&quot;bs0&quot;,
	BS0, BS1,

	&quot;bs1&quot;,
	BS1, BS1,

	&quot;33&quot;,
	CR1, ALL,

	&quot;tty33&quot;,
	CR1, ALL,

	&quot;37&quot;,
	FF1+CR2+TAB1+NL1, ALL,

	&quot;tty37&quot;,
	FF1+CR2+TAB1+NL1, ALL,

	&quot;05&quot;,
	NL2, ALL,

	&quot;vt05&quot;,
	NL2, ALL,

	&quot;tn&quot;,
	CR1, ALL,

	&quot;tn300&quot;,
	CR1, ALL,

	&quot;ti&quot;,
	CR2, ALL,

	&quot;ti700&quot;,
	CR2, ALL,

	&quot;tek&quot;,
	FF1, ALL,

	0,
	};

char	*arg;
int	mode[3];

struct { char lobyte, hibyte; };

main(argc, argv)
char	*argv[];
{
	int i;

	gtty(1, mode);
	if(argc == 1) {
		prmodes();
		exit(0);
	}
	while(--argc &gt; 0) {

		arg = *++argv;
		if (eq(&quot;ek&quot;))
			mode[1] = &#39;#@&#39;;
		if (eq(&quot;erase&quot;)) {
			mode[1].lobyte = **++argv;
			argc--;
		}
		if (eq(&quot;kill&quot;)) {
			mode[1].hibyte = **++argv;
			argc--;
		}
		for(i=0; speeds[i].string; i++)
			if(eq(speeds[i].string))
				mode[0] = speeds[i].speed;
		for(i=0; modes[i].string; i++)
			if(eq(modes[i].string)) {
				mode[2] =&amp; ~modes[i].reset;
				mode[2] =| modes[i].set;
			}
		if(arg)
			printf(&quot;unknown mode: %s\n&quot;, arg);
	}
	stty(1,mode);
}

eq(string)
char *string;
{
	int i;

	if(!arg)
		return(0);
	i = 0;
loop:
	if(arg[i] != string[i])
		return(0);
	if(arg[i++] != &#39;\0&#39;)
		goto loop;
	arg = 0;
	return(1);
}

prmodes()
{
	register m;

	if(mode[0].lobyte != mode[0].hibyte) {
		prspeed(&quot;input speed  &quot;, mode[0].lobyte);
		prspeed(&quot;output speed &quot;, mode[0].hibyte);
	} else
		prspeed(&quot;speed &quot;, mode[0].lobyte);
	printf(&quot;erase = &#39;%c&#39;; kill = &#39;%c&#39;\n&quot;, mode[1].lobyte, mode[1].hibyte);
	m = mode[2];
	if(m &amp; 0200) printf(&quot;even &quot;);
	if(m &amp; 0100) printf(&quot;odd &quot;);
	if(m &amp; 040) printf(&quot;raw &quot;);
	if(m &amp; 020) printf(&quot;-nl &quot;);
	if(m &amp; 010) printf(&quot;echo &quot;);
	if(m &amp; 04) printf(&quot;lcase &quot;);
	if(m &amp; 02) printf(&quot;-tabs &quot;);
	if(m &amp; 01) printf(&quot;hup &quot;);
	delay(m&gt;&gt;8, &quot;nl&quot;);
	delay(m&gt;&gt;10, &quot;tab&quot;);
	delay(m&gt;&gt;12, &quot;cr&quot;);
	delay((m&gt;&gt;14)&amp;1, &quot;ff&quot;);
	delay((m&gt;&gt;15)&amp;1, &quot;bs&quot;);
	printf(&quot;\n&quot;);
}

delay(m, s)
char *s;
{

	if(m =&amp; 3)
		printf(&quot;%s%d &quot;, s, m);
}

int	speed[]
{
	0,50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,0,0
};

prspeed(c, s)
{

	printf(&quot;%s%d baud\n&quot;, c, speed[s]);
}

putchar(c)
{

	write(2, &amp;c, 1);
}
