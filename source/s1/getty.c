#
/*
 * getty -- adapt to terminal speed on dialup, and call login
 */

/*
 * tty flags
 */
#define	HUPCL 01
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
#define	CR1	010000
#define	CR2	020000
#define	CR3	030000
#define	NL1	000400
#define	NL2	001000
#define	NL3	001400
#define	TAB1	002000
#define	TAB2	004000
#define	TAB3	006000
#define	FF1	040000

#define	ERASE	&#39;#&#39;
#define	KILL	&#39;@&#39;

/*
 * speeds
 */
#define	B110	3
#define	B150	5
#define	B300	7
#define	B9600	13

#define	SIGINT	2
#define	SIGQIT	3

struct	sgtty {
	char	sgispd, sgospd;
	char	sgerase, sgkill;
	int	sgflag;
} tmode;

struct	tab {
	int	tname;		/* this table name */
	int	nname;		/* successor table name */
	int	iflags;		/* initial flags */
	int	fflags;		/* final flags */
	int	ispeed;		/* input speed */
	int	ospeed;		/* output speed */
	char	*message;	/* login message */
} itab[] {

/* table &#39;0&#39;-1-2 300,150,110 */

	&#39;0&#39;, 1,
	ANYP+RAW+NL1+CR1, ANYP+ECHO+CR1,
	B300, B300,
	&quot;\n\r\033;\007login: &quot;,

	1, 2,
	ANYP+RAW+NL1+CR1, EVENP+ECHO+FF1+CR2+TAB1+NL1,
	B150, B150,
	&quot;\n\r\033:\006\006\017login: &quot;,

	2, &#39;0&#39;,
	ANYP+RAW+NL1+CR1, ANYP+ECHO+CRMOD+XTABS+LCASE+CR1,
	B110, B110,
	&quot;\n\rlogin: &quot;,

/* table &#39;-&#39; -- Console TTY 110 */
	&#39;-&#39;, &#39;-&#39;,
	ANYP+RAW+NL1+CR1, ANYP+ECHO+CRMOD+XTABS+LCASE+CR1,
	B110, B110,
	&quot;\n\rlogin: &quot;,

/* table &#39;1&#39; -- 150 */
	&#39;1&#39;, &#39;1&#39;,
	ANYP+RAW+NL1+CR1, EVENP+ECHO+FF1+CR2+TAB1+NL1,
	B150, B150,
	&quot;\n\r\033:\006\006\017login: &quot;,

/* table &#39;2&#39; -- 9600 */
	&#39;2&#39;, &#39;2&#39;,
	ANYP+RAW+NL1+CR1, ANYP+XTABS+ECHO+CRMOD+FF1,
	B9600, B9600,
	&quot;\n\r\033;login: &quot;,
};

#define	NITAB	sizeof itab/sizeof itab[0]

char	name[16];
int	crmod;
int	upper;
int	lower;

main(argc, argv)
char **argv;
{
	register struct tab *tabp;
	register tname;

/*
	signal(SIGINT, 1);
	signal(SIGQIT, 0);
*/
	tname = &#39;0&#39;;
	if (argc &gt; 1)
		tname = *argv[1];
	for (;;) {
		for(tabp = itab; tabp &lt; &amp;itab[NITAB]; tabp++)
			if(tabp-&gt;tname == tname)
				break;
		if(tabp &gt;= &amp;itab[NITAB])
			tabp = itab;
		tmode.sgispd = tabp-&gt;ispeed;
		tmode.sgospd = tabp-&gt;ospeed;
		tmode.sgflag = tabp-&gt;iflags;
		tmode.sgispd = tabp-&gt;ispeed;
		tmode.sgospd = tabp-&gt;ospeed;
		stty(0, &amp;tmode);
		puts(tabp-&gt;message);
		stty(0, &amp;tmode);
		if(getname()) {
			tmode.sgerase = ERASE;
			tmode.sgkill = KILL;
			tmode.sgflag = tabp-&gt;fflags;
			if(crmod)
				tmode.sgflag =| CRMOD;
			if(upper)
				tmode.sgflag =| LCASE;
			if(lower)
				tmode.sgflag =&amp; ~LCASE;
			stty(0, &amp;tmode);
			execl(&quot;/bin/login&quot;, &quot;login&quot;, name, 0);
			exit(1);
		}
		tname = tabp-&gt;nname;
	}
}

getname()
{
	register char *np;
	register c;
	static cs;

	crmod = 0;
	upper = 0;
	lower = 0;
	np = name;
	do {
		if (read(0, &amp;cs, 1) &lt;= 0)
			exit(0);
		if ((c = cs&amp;0177) == 0)
			return(0);
		write(1, &amp;cs, 1);
		if (c&gt;=&#39;a&#39; &amp;&amp; c &lt;=&#39;z&#39;)
			lower++;
		else if (c&gt;=&#39;A&#39; &amp;&amp; c&lt;=&#39;Z&#39;) {
			upper++;
			c =+ &#39;a&#39;-&#39;A&#39;;
		} else if (c==ERASE) {
			if (np &gt; name)
				np--;
			continue;
		} else if (c==KILL) {
			np = name;
			continue;
		}
		*np++ = c;
	} while (c!=&#39;\n&#39; &amp;&amp; c!=&#39;\r&#39; &amp;&amp; np &lt;= &amp;name[16]);
	*--np = 0;
	if (c == &#39;\r&#39;) {
		write(1, &quot;\n&quot;, 1);
		crmod++;
	} else
		write(1, &quot;\r&quot;, 1);
	return(1);
}

puts(as)
char *as;
{
	register char *s;

	s = as;
	while (*s)
		write(1, s++, 1);
}
