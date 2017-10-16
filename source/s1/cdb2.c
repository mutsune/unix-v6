#
/*
 * C debugger -- part 2
 */

char	ssymbol[];
int	dotinc;
int	dot;


psymoff(v, lim)
{
	register char *w;

	w = vallook(v);
	if (w &gt; lim) {
		printf(&quot;%.1o&quot;, v);
		return;
	}
	printf(&quot;%.8s&quot;, ssymbol);
	if (w)
		printf(&quot;+%d&quot;, w);
}

#define	ISP	1
#define	DOUBLE	0
#define	SINGLE	1
#define	SINGLW	2
#define	MULDIV	4
#define	BRANCH	5
#define	NOADDR	6
#define	FLTREV	7
#define	FLTNOR	8
#define	SPECL1	9
#define	SPECL2	10
#define	SPECL3	11
#define	SPECL4	12
#define	SPECL5	13
#define	SPECL6	14
#define	SPECL7	15
#define	SPECL8	18
#define	SPECL9	19

struct optab {
	int	mask;
	int	val;
	int	itype;
	char	*iname;
} optab[] {
	0107777, 0010000, DOUBLE, &quot;mov&quot;,
	0107777, 0020000, DOUBLE, &quot;cmp&quot;,
	0107777, 0030000, DOUBLE, &quot;bit&quot;,
	0107777, 0040000, DOUBLE, &quot;bic&quot;,
	0107777, 0050000, DOUBLE, &quot;bis&quot;,
	0007777, 0060000, DOUBLE, &quot;add&quot;,
	0007777, 0160000, DOUBLE, &quot;su&quot;,
	0100077, 0005000, SINGLE, &quot;clr&quot;,
	0100077, 0005100, SINGLE, &quot;com&quot;,
	0100077, 0005200, SINGLE, &quot;inc&quot;,
	0100077, 0005300, SINGLE, &quot;dec&quot;,
	0100077, 0005400, SINGLE, &quot;neg&quot;,
	0100077, 0005500, SINGLE, &quot;adc&quot;,
	0100077, 0005600, SINGLE, &quot;sbc&quot;,
	0100077, 0005700, SINGLE, &quot;tst&quot;,
	0100077, 0006000, SINGLE, &quot;ror&quot;,
	0100077, 0006100, SINGLE, &quot;rol&quot;,
	0100077, 0006200, SINGLE, &quot;asr&quot;,
	0100077, 0006300, SINGLE, &quot;asl&quot;,
	0000077, 0000100, SINGLE, &quot;jmp&quot;,
	0000077, 0000300, SINGLE, &quot;swab&quot;,
	0000077, 0170100, SINGLW, &quot;ldfps&quot;,
	0000077, 0170200, SINGLW, &quot;stfps&quot;,
	0000077, 0170300, SINGLW, &quot;stst&quot;,
	0000077, 0170400, SINGLW, &quot;clrf&quot;,
	0000077, 0170500, SINGLW, &quot;tstf&quot;,
	0000077, 0170600, SINGLW, &quot;absf&quot;,
	0000077, 0170700, SINGLW, &quot;negf&quot;,
	0000077, 0006700, SINGLW, &quot;sxt&quot;,
	0000077, 0006600, SINGLW, &quot;mtpi&quot;,
	0000077, 0106600, SINGLW, &quot;mtpd&quot;,
	0000077, 0006500, SINGLW, &quot;mfpi&quot;,
	0000077, 0106500, SINGLW, &quot;mfpd&quot;,
	0000777, 0070000, MULDIV, &quot;mul&quot;,
	0000777, 0071000, MULDIV, &quot;div&quot;,
	0000777, 0072000, MULDIV, &quot;ash&quot;,
	0000777, 0073000, MULDIV, &quot;ashc&quot;,
	0000377, 0000400, BRANCH, &quot;br&quot;,
	0000377, 0001000, BRANCH, &quot;bne&quot;,
	0000377, 0001400, BRANCH, &quot;beq&quot;,
	0000377, 0002000, BRANCH, &quot;bge&quot;,
	0000377, 0002400, BRANCH, &quot;blt&quot;,
	0000377, 0003000, BRANCH, &quot;bgt&quot;,
	0000377, 0003400, BRANCH, &quot;ble&quot;,
	0000377, 0100000, BRANCH, &quot;bpl&quot;,
	0000377, 0100400, BRANCH, &quot;bmi&quot;,
	0000377, 0101000, BRANCH, &quot;bhi&quot;,
	0000377, 0101400, BRANCH, &quot;blos&quot;,
	0000377, 0102000, BRANCH, &quot;bvc&quot;,
	0000377, 0102400, BRANCH, &quot;bvs&quot;,
	0000377, 0103000, BRANCH, &quot;bhis&quot;,
	0000377, 0103400, BRANCH, &quot;blo&quot;,
	0000000, 0000000, NOADDR, &quot;halt&quot;,
	0000000, 0000001, NOADDR, &quot;wait&quot;,
	0000000, 0000002, NOADDR, &quot;rti&quot;,
	0000000, 0000004, NOADDR, &quot;iot&quot;,
	0000000, 0000005, NOADDR, &quot;reset&quot;,
	0000377, 0171000, FLTREV, &quot;mulf&quot;,
	0000377, 0171400, FLTREV, &quot;modf&quot;,
	0000377, 0172000, FLTREV, &quot;addf&quot;,
	0000377, 0172400, FLTREV, &quot;movf&quot;,
	0000377, 0173000, FLTREV, &quot;subf&quot;,
	0000377, 0173400, FLTREV, &quot;cmpf&quot;,
	0000377, 0174000, FLTNOR, &quot;movf&quot;,
	0000377, 0174400, FLTREV, &quot;divf&quot;,
	0000377, 0175000, FLTNOR, &quot;movei&quot;,
	0000377, 0175400, FLTNOR, &quot;movfi&quot;,
	0000377, 0176000, FLTNOR, &quot;movfo&quot;,
	0000377, 0176400, FLTREV, &quot;movie&quot;,
	0000377, 0177000, FLTREV, &quot;movif&quot;,
	0000377, 0177400, FLTREV, &quot;movof&quot;,
	0000000, 0170000, NOADDR, &quot;cfcc&quot;,
	0000000, 0170001, NOADDR, &quot;setf&quot;,
	0000000, 0170002, NOADDR, &quot;seti&quot;,
	0000000, 0170011, NOADDR, &quot;setd&quot;,
	0000000, 0170012, NOADDR, &quot;setl&quot;,
	0000777, 0004000, SPECL1, &quot;jsr&quot;,
	0000777, 0074000, SPECL1, &quot;xor&quot;,
	0000007, 0000200, SPECL2, &quot;rts&quot;,
	0000017, 0000240, SPECL3, &quot;cflg&quot;,
	0000017, 0000260, SPECL3, &quot;sflg&quot;,
	0000377, 0104000, SPECL4, &quot;emt&quot;,
	0000377, 0104400, SPECL5, &quot;sys&quot;,
	0000077, 0006400, SPECL7, &quot;mark&quot;,
	0000777, 0077000, SPECL8, &quot;sob&quot;,
	0000007, 0000230, SPECL9, &quot;spl&quot;,
	0177777, 0000000, SPECL6, &quot;oct&quot;,
};

struct systab {
	int	argc;
	char	*sname;
} systab[] {
	1, &quot;indir&quot;,
	0, &quot;exit&quot;,
	0, &quot;fork&quot;,
	2, &quot;read&quot;,
	2, &quot;write&quot;,
	2, &quot;open&quot;,
	0, &quot;close&quot;,
	0, &quot;wait&quot;,
	2, &quot;creat&quot;,
	2, &quot;link&quot;,
	1, &quot;unlink&quot;,
	2, &quot;exec&quot;,
	1, &quot;chdir&quot;,
	0, &quot;time&quot;,
	3, &quot;mknod&quot;,
	2, &quot;chmod&quot;,
	2, &quot;chown&quot;,
	1, &quot;break&quot;,
	2, &quot;stat&quot;,
	2, &quot;seek&quot;,
	0, &quot;getpid&quot;,
	3, &quot;mount&quot;,
	1, &quot;umount&quot;,
	0, &quot;setuid&quot;,
	0, &quot;getuid&quot;,
	0, &quot;stime&quot;,
	3, &quot;ptrace&quot;,
	0, &quot;27&quot;,
	1, &quot;fstat&quot;,
	0, &quot;29&quot;,
	1, &quot;smdate&quot;,
	1, &quot;stty&quot;,
	1, &quot;gtty&quot;,
	0, &quot;33&quot;,
	0, &quot;nice&quot;,
	0, &quot;sleep&quot;,
	0, &quot;sync&quot;,
	1, &quot;kill&quot;,
	0, &quot;switch&quot;,
	0, &quot;39&quot;,
	0, &quot;40&quot;,
	0, &quot;dup&quot;,
	0, &quot;pipe&quot;,
	1, &quot;times&quot;,
	4, &quot;profil&quot;,
	0, &quot;45&quot;,
	0, &quot;setgid&quot;,
	0, &quot;getgid&quot;,
	2, &quot;signal&quot;,
	0, &quot;49&quot;,
	0, &quot;50&quot;,
	0, &quot;51&quot;,
	0, &quot;52&quot;,
	0, &quot;53&quot;,
	0, &quot;54&quot;,
	0, &quot;55&quot;,
	0, &quot;56&quot;,
	0, &quot;57&quot;,
	0, &quot;58&quot;,
	0, &quot;59&quot;,
	0, &quot;60&quot;,
	0, &quot;61&quot;,
	0, &quot;62&quot;,
	0, &quot;63&quot;,
};

char	*regname[] { &quot;r0&quot;, &quot;r1&quot;, &quot;r2&quot;, &quot;r3&quot;, &quot;r4&quot;, &quot;r5&quot;, &quot;sp&quot;, &quot;pc&quot;};

printins(f)
{
	register ins, w;
	register struct optab *p;

	dotinc = 2;
	ins = cget(dot, ISP);
	if (vallook(dot)==0)
		printf(&quot;%.8s:&quot;, ssymbol);
	printf(&quot;\t&quot;);
	for (p=optab;; p++)
		if ((ins &amp; ~p-&gt;mask) == p-&gt;val)
			break;
	printf(&quot;%s&quot;, p-&gt;iname);
	switch (p-&gt;itype) {

	/* rts */
	case SPECL2:
		ins =&amp; 07;

	case SINGLE:
		if (ins &lt; 0)
			printf(&quot;b&quot;);

	case SINGLW:
		printf(&quot;\t&quot;);
		paddr(ins);
		return;

	case FLTREV:
		ins =&amp; 0377;

	case MULDIV:
		ins = ((ins&gt;&gt;6)&amp;07) | ((ins&lt;&lt;6)&amp;07700);
		goto doub;

	case FLTNOR:
		ins =&amp; 0377;

	/* jsr, xor */
	case SPECL1:
		ins =&amp; 0777;
		goto doub;

	case DOUBLE:
		if (ins&lt;0)
			printf(&quot;b&quot;);
	doub:
		printf(&quot;\t&quot;);
		paddr(ins&gt;&gt;6);
		printf(&quot;,&quot;);
		paddr(ins);
		return;

	case NOADDR:
		return;

	/* sob */
	case SPECL8:
		printf(&quot;\t&quot;);
		paddr((ins&gt;&gt;6)&amp;07);
		printf(&quot;,&quot;);
		ins = - (ins&amp;077);
		goto bran;

	case BRANCH:
		printf(&quot;\t&quot;);
	bran:
		ins =&amp; 0377;
		if (ins&amp;0200)
			ins =| 0177400;
		ins = dot + (ins&lt;&lt;1) + 2;
		psymoff(ins, 010000);
		return;

	/* emt */
	case SPECL4:
		ins =&amp; 0377;
	/* mark */
	case SPECL7:
		ins =&amp; 077;
	/* spl */
	case SPECL9:
		ins =&amp; 07;
		printf(&quot;\t%d&quot;, ins);
		return;

	/* sys */
	case SPECL5:
		printf(&quot;\t%s&quot;, systab[ins =&amp; 077].sname);
		if (ins==0 &amp;&amp; f==0) {	/* indir */
			w = dot;
			dot = cget(dot+2, ISP);
			printf(&quot; {&quot;);
			printins(1);
			printf(&quot;}&quot;);
			dotinc = 4;
			dot = w;
			return;
		}
		w = systab[ins].argc;
		while (w--) {
			printf(&quot;; &quot;);
			psymoff(cget(dot+dotinc, ISP), 010000);
			dotinc =+ 2;
		}
		return;

	default:
		printf(&quot;\t%.1o&quot;, ins);
	}
}

paddr(aa)
{
	register a, r;

	a = aa;
	r = a&amp;07;
	a =&amp; 070;
	if (r==7 &amp;&amp; a&amp;020) {
		if (a&amp;010)
			printf(&quot;*&quot;);
		if (a&amp;040)
			psymoff(cget(dot+dotinc, ISP)+dot+dotinc+2, 010000);
		else {
			printf(&quot;$&quot;);
			psymoff(cget(dot+dotinc, ISP), 010000);
		}
		dotinc =+ 2;
		return;
	}
	r = regname[r];
	switch (a) {
	/* r */
	case 000:
		printf(&quot;%s&quot;, r);
		return;

	/* (r) */
	case 010:
		printf(&quot;(%s)&quot;, r);
		return;

	/* *(r)+ */
	case 030:
		printf(&quot;*&quot;);

	/* (r)+ */
	case 020:
		printf(&quot;(%s)+&quot;, r);
		return;

	/* *-(r) */
	case 050:
		printf(&quot;*&quot;);

	/* -(r) */
	case 040:
		printf(&quot;-(%s)&quot;, r);
		return;

	/* *x(r) */
	case 070:
		printf(&quot;*&quot;);

	/* x(r) */
	case 060:
		psymoff(cget(dot+dotinc, ISP), 010000);
		dotinc =+ 2;
		printf(&quot;(%s)&quot;, r);
		return;
	}
}
