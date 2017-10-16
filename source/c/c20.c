#
/*
 *	 C object code improver
 */

#include &quot;c2h.c&quot;

struct optab optab[] {
	&quot;jbr&quot;,	JBR,
	&quot;jeq&quot;,	CBR | JEQ&lt;&lt;8,
	&quot;jne&quot;,	CBR | JNE&lt;&lt;8,
	&quot;jle&quot;,	CBR | JLE&lt;&lt;8,
	&quot;jge&quot;,	CBR | JGE&lt;&lt;8,
	&quot;jlt&quot;,	CBR | JLT&lt;&lt;8,
	&quot;jgt&quot;,	CBR | JGT&lt;&lt;8,
	&quot;jlo&quot;,	CBR | JLO&lt;&lt;8,
	&quot;jhi&quot;,	CBR | JHI&lt;&lt;8,
	&quot;jlos&quot;,	CBR | JLOS&lt;&lt;8,
	&quot;jhis&quot;,	CBR | JHIS&lt;&lt;8,
	&quot;jmp&quot;,	JMP,
	&quot;.globl&quot;,EROU,
	&quot;mov&quot;,	MOV,
	&quot;clr&quot;,	CLR,
	&quot;com&quot;,	COM,
	&quot;inc&quot;,	INC,
	&quot;dec&quot;,	DEC,
	&quot;neg&quot;,	NEG,
	&quot;tst&quot;,	TST,
	&quot;asr&quot;,	ASR,
	&quot;asl&quot;,	ASL,
	&quot;sxt&quot;,	SXT,
	&quot;cmp&quot;,	CMP,
	&quot;add&quot;,	ADD,
	&quot;sub&quot;,	SUB,
	&quot;bit&quot;,	BIT,
	&quot;bic&quot;,	BIC,
	&quot;bis&quot;,	BIS,
	&quot;mul&quot;,	MUL,
	&quot;ash&quot;,	ASH,
	&quot;xor&quot;,	XOR,
	&quot;.text&quot;,TEXT,
	&quot;.data&quot;,DATA,
	&quot;.bss&quot;,	BSS,
	&quot;.even&quot;,EVEN,
	&quot;movf&quot;,	MOVF,
	&quot;movof&quot;,MOVOF,
	&quot;movfo&quot;,MOVFO,
	&quot;addf&quot;,	ADDF,
	&quot;subf&quot;,	SUBF,
	&quot;divf&quot;,	DIVF,
	&quot;mulf&quot;,	MULF,
	&quot;clrf&quot;,	CLRF,
	&quot;cmpf&quot;,	CMPF,
	&quot;negf&quot;,	NEGF,
	&quot;tstf&quot;,	TSTF,
	&quot;cfcc&quot;,	CFCC,
	&quot;sob&quot;,	SOB,
	&quot;jsr&quot;,	JSR,
	&quot;.end&quot;,	END,
	0,	0};

char	revbr[] { JNE, JEQ, JGT, JLT, JGE, JLE, JHIS, JLOS, JHI, JLO };
int	isn	20000;

main(argc, argv)
char **argv;
{
	register int niter, maxiter, isend;
	extern end;
	extern fin, fout;
	int nflag;

	if (argc&gt;1 &amp;&amp; argv[1][0]==&#39;+&#39;) {
		argc--;
		argv++;
		debug++;
	}
	if (argc&gt;1 &amp;&amp; argv[1][0]==&#39;-&#39;) {
		argc--;
		argv++;
		nflag++;
	}
	if (argc&gt;1) {
		if ((fin = open(argv[1], 0)) &lt; 0) {
			printf(&quot;C2: can&#39;t find %s\n&quot;, argv[1]);
			exit(1);
		}
	} else
		fin = dup(0);
	if (argc&gt;2) {
		if ((fout = creat(argv[2], 0666)) &lt; 0) {
			fout = 1;
			printf(&quot;C2: can&#39;t create %s\n&quot;, argv[2]);
			exit(1);
		}
	} else
		fout = dup(1);
	lasta = firstr = lastr = sbrk(2);
	maxiter = 0;
	opsetup();
	do {
		isend = input();
		movedat();
		niter = 0;
		do {
			refcount();
			do {
				iterate();
				clearreg();
				niter++;
			} while (nchange);
			comjump();
			rmove();
		} while (nchange || jumpsw());
		addsob();
		output();
		if (niter &gt; maxiter)
			maxiter = niter;
		lasta = firstr;
	} while (isend);
	flush();
	fout = 2;
	if (nflag) {
		printf(&quot;%d iterations\n&quot;, maxiter);
		printf(&quot;%d jumps to jumps\n&quot;, nbrbr);
		printf(&quot;%d inst. after jumps\n&quot;, iaftbr);
		printf(&quot;%d jumps to .+2\n&quot;, njp1);
		printf(&quot;%d redundant labels\n&quot;, nrlab);
		printf(&quot;%d cross-jumps\n&quot;, nxjump);
		printf(&quot;%d code motions\n&quot;, ncmot);
		printf(&quot;%d branches reversed\n&quot;, nrevbr);
		printf(&quot;%d redundant moves\n&quot;, redunm);
		printf(&quot;%d simplified addresses\n&quot;, nsaddr);
		printf(&quot;%d loops inverted\n&quot;, loopiv);
		printf(&quot;%d redundant jumps\n&quot;, nredunj);
		printf(&quot;%d common seqs before jmp&#39;s\n&quot;, ncomj);
		printf(&quot;%d skips over jumps\n&quot;, nskip);
		printf(&quot;%d sob&#39;s added\n&quot;, nsob);
		printf(&quot;%d redundant tst&#39;s\n&quot;, nrtst);
		printf(&quot;%d literals eliminated\n&quot;, nlit);
		printf(&quot;%dK core\n&quot;, ((lastr+01777)&gt;&gt;10)&amp;077);
		flush();
	}
	exit(0);
}

input()
{
	register struct node *p, *lastp;
	register int op;

	lastp = &amp;first;
	for (;;) {
		op = getline();
		switch (op.op) {
	
		case LABEL:
			p = alloc(sizeof first);
			if (line[0] == &#39;L&#39;) {
				p-&gt;combop = LABEL;
				p-&gt;labno = getnum(line+1);
				p-&gt;code = 0;
			} else {
				p-&gt;combop = DLABEL;
				p-&gt;labno = 0;
				p-&gt;code = copy(line);
			}
			break;
	
		case JBR:
		case CBR:
		case JMP:
		case JSW:
			p = alloc(sizeof first);
			p-&gt;combop = op;
			if (*curlp==&#39;L&#39; &amp;&amp; (p-&gt;labno = getnum(curlp+1)))
				p-&gt;code = 0;
			else {
				p-&gt;labno = 0;
				p-&gt;code = copy(curlp);
			}
			break;

		default:
			p = alloc(sizeof first);
			p-&gt;combop = op;
			p-&gt;labno = 0;
			p-&gt;code = copy(curlp);
			break;

		}
		p-&gt;forw = 0;
		p-&gt;back = lastp;
		lastp-&gt;forw = p;
		lastp = p;
		p-&gt;ref = 0;
		if (op==EROU)
			return(1);
		if (op==END)
			return(0);
	}
}

getline()
{
	register char *lp;
	register c;

	lp = line;
	while (c = getchar()) {
		if (c==&#39;:&#39;) {
			*lp++ = 0;
			return(LABEL);
		}
		if (c==&#39;\n&#39;) {
			*lp++ = 0;
			return(oplook());
		}
		*lp++ = c;
	}
	*lp++ = 0;
	return(END);
}

getnum(ap)
char *ap;
{
	register char *p;
	register n, c;

	p = ap;
	n = 0;
	while ((c = *p++) &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;)
		n = n*10 + c - &#39;0&#39;;
	if (*--p != 0)
		return(0);
	return(n);
}

output()
{
	register struct node *t;
	register struct optab *op;
	register int byte;

	t = &amp;first;
	while (t = t-&gt;forw) switch (t-&gt;op) {

	case END:
		return;

	case LABEL:
		printf(&quot;L%d:&quot;, t-&gt;labno);
		continue;

	case DLABEL:
		printf(&quot;%s:&quot;, t-&gt;code);
		continue;

	default:
		if ((byte = t-&gt;subop) == BYTE)
			t-&gt;subop = 0;
		for (op = optab; op-&gt;opstring!=0; op++) 
			if (op-&gt;opcode == t-&gt;combop) {
				printf(&quot;%s&quot;, op-&gt;opstring);
				if (byte==BYTE)
					printf(&quot;b&quot;);
				break;
			}
		if (t-&gt;code) {
			reducelit(t);
			printf(&quot;\t%s\n&quot;, t-&gt;code);
		} else if (t-&gt;op==JBR || t-&gt;op==CBR)
			printf(&quot;\tL%d\n&quot;, t-&gt;labno);
		else
			printf(&quot;\n&quot;);
		continue;

	case JSW:
		printf(&quot;L%d\n&quot;, t-&gt;labno);
		continue;

	case SOB:
		printf(&quot;sob	%s,L%d\n&quot;, t-&gt;code, t-&gt;labno);
		continue;

	case 0:
		if (t-&gt;code)
			printf(&quot;%s&quot;, t-&gt;code);
		printf(&quot;\n&quot;);
		continue;
	}
}

/*
 * Notice addresses of the form
 * $xx,xx(r)
 * and replace them with (pc),xx(r)
 *     -- Thanx and a tip of the Hatlo hat to Bliss-11.
 */
reducelit(at)
struct node *at;
{
	register char *c1, *c2;
	char *c2s;
	register struct node *t;

	t = at;
	if (*t-&gt;code != &#39;$&#39;)
		return;
	c1 = t-&gt;code;
	while (*c1 != &#39;,&#39;)
		if (*c1++ == &#39;\0&#39;)
			return;
	c2s = c1;
	c1++;
	if (*c1==&#39;*&#39;)
		c1++;
	c2 = t-&gt;code+1;
	while (*c1++ == *c2++);
	if (*--c1!=&#39;(&#39; || *--c2!=&#39;,&#39;)
		return;
	t-&gt;code = copy(&quot;(pc)&quot;, c2s);
	nlit++;
}

copy(ap)
char *ap;
{
	register char *p, *np;
	char *onp;
	register n;
	int na;

	na = nargs();
	p = ap;
	n = 0;
	if (*p==0)
		return(0);
	do
		n++;
	while (*p++);
	if (na&gt;1) {
		p = (&amp;ap)[1];
		while (*p++)
			n++;
	}
	onp = np = alloc(n);
	p = ap;
	while (*np++ = *p++);
	if (na&gt;1) {
		p = (&amp;ap)[1];
		np--;
		while (*np++ = *p++);
	}
	return(onp);
}

opsetup()
{
	register struct optab *optp, **ophp;
	register char *p;

	for (optp = optab; p = optp-&gt;opstring; optp++) {
		ophp = &amp;ophash[(((p[0]&lt;&lt;3)+(p[1]&lt;&lt;1)+p[2])&amp;077777) % OPHS];
		while (*ophp++)
			if (ophp &gt; &amp;ophash[OPHS])
				ophp = ophash;
		*--ophp = optp;
	}
}

oplook()
{
	register struct optab *optp;
	register char *lp, *op;
	static char tmpop[32];
	struct optab **ophp;

	op = tmpop;
	for (lp = line; *lp &amp;&amp; *lp!=&#39; &#39; &amp;&amp; *lp!=&#39;\t&#39;;)
		*op++ = *lp++;
	*op++ = 0;
	while (*lp==&#39;\t&#39; || *lp==&#39; &#39;)
		lp++;
	curlp = lp;
	ophp = &amp;ophash[(((tmpop[0]&lt;&lt;3)+(tmpop[1]&lt;&lt;1)+tmpop[2])&amp;077777) % OPHS];
	while (optp = *ophp) {
		op = optp-&gt;opstring;
		lp = tmpop;
		while (*lp == *op++)
			if (*lp++ == 0)
				return(optp-&gt;opcode);
		if (*lp++==&#39;b&#39; &amp;&amp; *lp++==0 &amp;&amp; *--op==0)
			return(optp-&gt;opcode + (BYTE&lt;&lt;8));
		ophp++;
		if (ophp &gt;= &amp;ophash[OPHS])
			ophp = ophash;
	}
	if (line[0]==&#39;L&#39;) {
		lp = &amp;line[1];
		while (*lp)
			if (*lp&lt;&#39;0&#39; || *lp++&gt;&#39;9&#39;)
				return(0);
		curlp = line;
		return(JSW);
	}
	curlp = line;
	return(0);
}

refcount()
{
	register struct node *p, *lp;
	static struct node *labhash[LABHS];
	register struct node **hp;

	for (hp = labhash; hp &lt; &amp;labhash[LABHS];)
		*hp++ = 0;
	for (p = first.forw; p!=0; p = p-&gt;forw)
		if (p-&gt;op==LABEL) {
			labhash[p-&gt;labno % LABHS] = p;
			p-&gt;refc = 0;
		}
	for (p = first.forw; p!=0; p = p-&gt;forw) {
		if (p-&gt;op==JBR || p-&gt;op==CBR || p-&gt;op==JSW) {
			p-&gt;ref = 0;
			lp = labhash[p-&gt;labno % LABHS];
			if (lp==0 || p-&gt;labno!=lp-&gt;labno)
			for (lp = first.forw; lp!=0; lp = lp-&gt;forw) {
				if (lp-&gt;op==LABEL &amp;&amp; p-&gt;labno==lp-&gt;labno)
					break;
			}
			if (lp) {
				hp = nonlab(lp)-&gt;back;
				if (hp!=lp) {
					p-&gt;labno = hp-&gt;labno;
					lp = hp;
				}
				p-&gt;ref = lp;
				lp-&gt;refc++;
			}
		}
	}
	for (p = first.forw; p!=0; p = p-&gt;forw)
		if (p-&gt;op==LABEL &amp;&amp; p-&gt;refc==0
		 &amp;&amp; (lp = nonlab(p))-&gt;op &amp;&amp; lp-&gt;op!=JSW)
			decref(p);
}

iterate()
{
	register struct node *p, *rp, *p1;

	nchange = 0;
	for (p = first.forw; p!=0; p = p-&gt;forw) {
		if ((p-&gt;op==JBR||p-&gt;op==CBR||p-&gt;op==JSW) &amp;&amp; p-&gt;ref) {
			rp = nonlab(p-&gt;ref);
			if (rp-&gt;op==JBR &amp;&amp; rp-&gt;labno &amp;&amp; p!=rp) {
				nbrbr++;
				p-&gt;labno = rp-&gt;labno;
				decref(p-&gt;ref);
				rp-&gt;ref-&gt;refc++;
				p-&gt;ref = rp-&gt;ref;
				nchange++;
			}
		}
		if (p-&gt;op==CBR &amp;&amp; (p1 = p-&gt;forw)-&gt;op==JBR) {
			rp = p-&gt;ref;
			do
				rp = rp-&gt;back;
			while (rp-&gt;op==LABEL);
			if (rp==p1) {
				decref(p-&gt;ref);
				p-&gt;ref = p1-&gt;ref;
				p-&gt;labno = p1-&gt;labno;
				p1-&gt;forw-&gt;back = p;
				p-&gt;forw = p1-&gt;forw;
				p-&gt;subop = revbr[p-&gt;subop];
				nchange++;
				nskip++;
			}
		}
		if (p-&gt;op==JBR || p-&gt;op==JMP) {
			while (p-&gt;forw &amp;&amp; p-&gt;forw-&gt;op!=LABEL
				&amp;&amp; p-&gt;forw-&gt;op!=EROU &amp;&amp; p-&gt;forw-&gt;op!=END) {
				nchange++;
				iaftbr++;
				if (p-&gt;forw-&gt;ref)
					decref(p-&gt;forw-&gt;ref);
				p-&gt;forw = p-&gt;forw-&gt;forw;
				p-&gt;forw-&gt;back = p;
			}
			rp = p-&gt;forw;
			while (rp &amp;&amp; rp-&gt;op==LABEL) {
				if (p-&gt;ref == rp) {
					p-&gt;back-&gt;forw = p-&gt;forw;
					p-&gt;forw-&gt;back = p-&gt;back;
					p = p-&gt;back;
					decref(rp);
					nchange++;
					njp1++;
					break;
				}
				rp = rp-&gt;forw;
			}
			xjump(p);
			p = codemove(p);
		}
	}
}

xjump(ap)
{
	register int *p1, *p2, *p3;
	int nxj;

	nxj = 0;
	p1 = ap;
	if ((p2 = p1-&gt;ref)==0)
		return(0);
	for (;;) {
		while ((p1 = p1-&gt;back) &amp;&amp; p1-&gt;op==LABEL);
		while ((p2 = p2-&gt;back) &amp;&amp; p2-&gt;op==LABEL);
		if (!equop(p1, p2) || p1==p2)
			return(nxj);
		p3 = insertl(p2);
		p1-&gt;combop = JBR;
		p1-&gt;ref = p3;
		p1-&gt;labno = p3-&gt;labno;
		p1-&gt;code = 0;
		nxj++;
		nxjump++;
		nchange++;
	}
}

insertl(ap)
struct node *ap;
{
	register struct node *lp, *op;

	op = ap;
	if (op-&gt;op == LABEL) {
		op-&gt;refc++;
		return(op);
	}
	if (op-&gt;back-&gt;op == LABEL) {
		op = op-&gt;back;
		op-&gt;refc++;
		return(op);
	}
	lp = alloc(sizeof first);
	lp-&gt;combop = LABEL;
	lp-&gt;labno = isn++;
	lp-&gt;ref = 0;
	lp-&gt;code = 0;
	lp-&gt;refc = 1;
	lp-&gt;back = op-&gt;back;
	lp-&gt;forw = op;
	op-&gt;back-&gt;forw = lp;
	op-&gt;back = lp;
	return(lp);
}

codemove(ap)
struct node *ap;
{
	register struct node *p1, *p2, *p3;
	struct node *t, *tl;
	int n;

	p1 = ap;
	if (p1-&gt;op!=JBR || (p2 = p1-&gt;ref)==0)
		return(p1);
	while (p2-&gt;op == LABEL)
		if ((p2 = p2-&gt;back) == 0)
			return(p1);
	if (p2-&gt;op!=JBR &amp;&amp; p2-&gt;op!=JMP)
		goto ivloop;
	p2 = p2-&gt;forw;
	p3 = p1-&gt;ref;
	while (p3) {
		if (p3-&gt;op==JBR || p3-&gt;op==JMP) {
			if (p1==p3)
				return(p1);
			ncmot++;
			nchange++;
			p1-&gt;back-&gt;forw = p2;
			p1-&gt;forw-&gt;back = p3;
			p2-&gt;back-&gt;forw = p3-&gt;forw;
			p3-&gt;forw-&gt;back = p2-&gt;back;
			p2-&gt;back = p1-&gt;back;
			p3-&gt;forw = p1-&gt;forw;
			decref(p1-&gt;ref);
			return(p2);
		} else
			p3 = p3-&gt;forw;
	}
	return(p1);
ivloop:
	if (p1-&gt;forw-&gt;op!=LABEL)
		return(p1);
	p3 = p2 = p2-&gt;forw;
	n = 16;
	do {
		if ((p3 = p3-&gt;forw) == 0 || p3==p1 || --n==0)
			return(p1);
	} while (p3-&gt;op!=CBR || p3-&gt;labno!=p1-&gt;forw-&gt;labno);
	do 
		if ((p1 = p1-&gt;back) == 0)
			return(ap);
	while (p1!=p3);
	p1 = ap;
	tl = insertl(p1);
	p3-&gt;subop = revbr[p3-&gt;subop];
	decref(p3-&gt;ref);
	p2-&gt;back-&gt;forw = p1;
	p3-&gt;forw-&gt;back = p1;
	p1-&gt;back-&gt;forw = p2;
	p1-&gt;forw-&gt;back = p3;
	t = p1-&gt;back;
	p1-&gt;back = p2-&gt;back;
	p2-&gt;back = t;
	t = p1-&gt;forw;
	p1-&gt;forw = p3-&gt;forw;
	p3-&gt;forw = t;
	p2 = insertl(p1-&gt;forw);
	p3-&gt;labno = p2-&gt;labno;
	p3-&gt;ref = p2;
	decref(tl);
	if (tl-&gt;refc&lt;=0)
		nrlab--;
	loopiv++;
	nchange++;
	return(p3);
}

comjump()
{
	register struct node *p1, *p2, *p3;

	for (p1 = first.forw; p1!=0; p1 = p1-&gt;forw)
		if (p1-&gt;op==JBR &amp;&amp; (p2 = p1-&gt;ref) &amp;&amp; p2-&gt;refc &gt; 1)
			for (p3 = p1-&gt;forw; p3!=0; p3 = p3-&gt;forw)
				if (p3-&gt;op==JBR &amp;&amp; p3-&gt;ref == p2)
					backjmp(p1, p3);
}

backjmp(ap1, ap2)
struct node *ap1, *ap2;
{
	register struct node *p1, *p2, *p3;

	p1 = ap1;
	p2 = ap2;
	for(;;) {
		while ((p1 = p1-&gt;back) &amp;&amp; p1-&gt;op==LABEL);
		p2 = p2-&gt;back;
		if (equop(p1, p2)) {
			p3 = insertl(p1);
			p2-&gt;back-&gt;forw = p2-&gt;forw;
			p2-&gt;forw-&gt;back = p2-&gt;back;
			p2 = p2-&gt;forw;
			decref(p2-&gt;ref);
			p2-&gt;labno = p3-&gt;labno;
			p2-&gt;ref = p3;
			nchange++;
			ncomj++;
		} else
			return;
	}
}
