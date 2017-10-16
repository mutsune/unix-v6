#
/*

	    	C compiler, part 2


*/

#include &quot;c1h.c&quot;

char	maprel[] {	EQUAL, NEQUAL, GREATEQ, GREAT, LESSEQ,
			LESS, GREATQP, GREATP, LESSEQP, LESSP
};

char	notrel[] {	NEQUAL, EQUAL, GREAT, GREATEQ, LESS,
			LESSEQ, GREATP, GREATQP, LESSP, LESSEQP
};

struct tconst czero { CON, INT, 0, 0};
struct tconst cone  { CON, INT, 0, 1};
struct tconst fczero { SFCON, DOUBLE, 0, 0 };

struct	table	*cregtab;

int	nreg	3;
int	isn	10000;
int	namsiz	8;

main(argc, argv)
char *argv[];
{
	extern fout;

	if (argc&lt;4) {
		error(&quot;Arg count&quot;);
		exit(1);
	}
	if(fopen(argv[1], ascbuf)&lt;0) {
		error(&quot;Missing temp file&quot;);
		exit(1);
	}
	if ((fout = creat(argv[3], 0666)) &lt; 0) {
		error(&quot;Can&#39;t create %s&quot;, argv[3]);
		exit(1);
	}
	spacep = treespace;
	getree();
	/*
	 * If any floating-point instructions
	 * were used, generate a reference which
	 * pulls in the floating-point part of printf.
	 */
	if (nfloat)
		printf(&quot;.globl	fltused\n&quot;);
	/*
	 * tack on the string file.
	 */
	close(ascbuf[0]);
	if (fopen(argv[2], ascbuf)&lt;0) {
		error(&quot;Missing temp file&quot;);
		exit(1);
	}
	printf(&quot;.globl\n.data\n&quot;);
	getree();
	flush();
	exit(nerror!=0);
}

/*
 * Given a tree, a code table, and a
 * count of available registers, find the code table
 * for the appropriate operator such that the operands
 * are of the right type and the number of registers
 * required is not too large.
 * Return a ptr to the table entry or 0 if none found.
 */
char *match(atree, table, nrleft)
struct tnode *atree;
struct table *table;
{
	int op, d1, d2, t1, t2, dope;
	struct tnode *p2;
	register struct tnode *p1, *tree;
	register struct optab *opt;

	if ((tree=atree)==0)
		return(0);
	if (table==lsptab)
		table = sptab;
	if ((op = tree-&gt;op)==0)
		return(0);
	dope = opdope[op];
	if ((dope&amp;LEAF) == 0)
		p1 = tree-&gt;tr1;
	else
		p1 = tree;
	t1 = p1-&gt;type;
	d1 = dcalc(p1, nrleft);
	if ((dope&amp;BINARY)!=0) {
		p2 = tree-&gt;tr2;
		/*
		 * If a subtree starts off with a conversion operator,
		 * try for a match with the conversion eliminated.
		 * E.g. int = double can be done without generating
		 * the converted int in a register by
		 * movf double,fr0; movfi fr0,int .
		 */
		if(opdope[p1-&gt;op]&amp;CNVRT &amp;&amp; (opdope[p2-&gt;op]&amp;CNVRT)==0) {
			tree-&gt;tr1 = p1-&gt;tr1;
			if (opt = match(tree, table, nrleft))
				return(opt);
			tree-&gt;tr1 = p1;
		} else if (opdope[p2-&gt;op]&amp;CNVRT &amp;&amp; (opdope[p1-&gt;op]&amp;CNVRT)==0) {
			tree-&gt;tr2 = p2-&gt;tr1;
			if (opt = match(tree, table, nrleft))
				return(opt);
			tree-&gt;tr2 = p2;
		}
		t2 = p2-&gt;type;
		d2 = dcalc(p2, nrleft);
	}
	for (; table-&gt;op!=op; table++)
		if (table-&gt;op==0)
			return(0);
	for (opt = table-&gt;tabp; opt-&gt;tabdeg1!=0; opt++) {
		if (d1 &gt; (opt-&gt;tabdeg1&amp;077)
		 || (opt-&gt;tabdeg1 &gt;= 0100 &amp;&amp; (p1-&gt;op != STAR)))
			continue;
		if (notcompat(p1, opt-&gt;tabtyp1, op)) {
			continue;
		}
		if ((opdope[op]&amp;BINARY)!=0 &amp;&amp; p2!=0) {
			if (d2 &gt; (opt-&gt;tabdeg2&amp;077)
			 || (opt-&gt;tabdeg2 &gt;= 0100) &amp;&amp; (p2-&gt;op != STAR) )
				continue;
			if (notcompat(p2,opt-&gt;tabtyp2, 0))
				continue;
		}
		return(opt);
	}
	return(0);
}

/*
 * Given a tree, a code table, and a register,
 * produce code to evaluate the tree with the appropriate table.
 * Registers reg and upcan be used.
 * If there is a value, it is desired that it appear in reg.
 * The routine returns the register in which the value actually appears.
 * This routine must work or there is an error.
 * If the table called for is cctab, sptab, or efftab,
 * and tree can&#39;t be done using the called-for table,
 * another try is made.
 * If the tree can&#39;t be compiled using cctab, regtab is
 * used and a &quot;tst&quot; instruction is produced.
 * If the tree can&#39;t be compiled using sptab,
 * regtab is used and the register is pushed on the stack.
 * If the tree can&#39;t be compiled using efftab,
 * just use regtab.
 * Regtab must succeed or an &quot;op not found&quot; error results.
 *
 * A number of special cases are recognized, and
 * there is an interaction with the optimizer routines.
 */
rcexpr(atree, atable, reg)
struct tnode *atree;
struct table *atable;
{
	register r;
	int modf, nargs, recurf;
	register struct tnode *tree;
	register struct table *table;

	table = atable;
	recurf = 0;
	if (reg&lt;0) {
		recurf++;
		reg = ~reg;
		if (reg&gt;=020) {
			reg =- 020;
			recurf++;
		}
	}
	if((tree=atree)==0)
		return(0);
	switch (tree-&gt;op)  {

	/*
	 * A conditional branch
	 */
	case CBRANCH:
		cbranch(optim(tree-&gt;btree), tree-&gt;lbl, tree-&gt;cond, 0);
		return(0);

	/*
	 * An initializing expression
	 */
	case INIT:
		if (tree-&gt;tr1-&gt;op == AMPER)
			tree-&gt;tr1 = tree-&gt;tr1-&gt;tr1;
		if (tree-&gt;tr1-&gt;op==NAME)
			pname(tree-&gt;tr1);
		else if (tree-&gt;tr1==CON)
			psoct(tree-&gt;tr1-&gt;value);
		else
			error(&quot;Illegal initialization&quot;);
		putchar(&#39;\n&#39;);
		return(0);

	/*
	 * Put the value of an expression in r0,
	 * for a switch or a return
	 */
	case RFORCE:
		if((r=rcexpr(tree-&gt;tr1, regtab, reg)) != 0)
			printf(&quot;mov%c	r%d,r0\n&quot;, isfloat(tree-&gt;tr1), r);
		return(0);

	/*
	 * sequential execution
	 */
	case COMMA:
		rcexpr(tree-&gt;tr1, efftab, reg);
		atree = tree = tree-&gt;tr2;
		break;

	/*
	 * In the generated &amp;~ operator,
	 * fiddle things so a PDP-11 &quot;bit&quot;
	 * instruction will be produced when cctab is used.
	 */
	case NAND:
		if (table==cctab) {
			tree-&gt;op = TAND;
			tree-&gt;tr2 = optim(block(1, COMPL, INT, 0, tree-&gt;tr2));
		}
		break;


	/*
	 * Handle a subroutine call. It has to be done
	 * here because if cexpr got called twice, the
	 * arguments might be compiled twice.
	 * There is also some fiddling so the
	 * first argument, in favorable circumstances,
	 * goes to (sp) instead of -(sp), reducing
	 * the amount of stack-popping.
	 */
	case CALL:
		r = 0;
		nargs = 0;
		modf = 0;
		if (tree-&gt;tr1-&gt;op!=NAME) {	/* get nargs right */
			nargs++;
			nstack++;
		}
		tree = tree-&gt;tr2;
		if(tree-&gt;op) {
			while (tree-&gt;op==COMMA) {
				r =+ comarg(tree-&gt;tr2, &amp;modf);
				tree = tree-&gt;tr1;
				nargs++;
			}
			r =+ comarg(tree, &amp;modf);
			nargs++;
		}
		tree = atree;
		tree-&gt;op = CALL2;
		if (modf &amp;&amp; tree-&gt;tr1-&gt;op==NAME &amp;&amp; tree-&gt;tr1-&gt;class==EXTERN)
			tree-&gt;op = CALL1;
		cexpr(tree, regtab, reg);
		popstk(r);
		nstack =- nargs;
		if (table==efftab || table==regtab)
			return(0);
		r = 0;
		goto fixup;

	/*
	 * Longs need special treatment.
	 */
	case ASLSH:
	case LSHIFT:
		if (tree-&gt;type==LONG) {
			if (tree-&gt;tr2-&gt;op==ITOL)
				tree-&gt;tr2 = tree-&gt;tr2-&gt;tr1;
			if (tree-&gt;op==ASLSH)
				tree-&gt;op = ASLSHL;
			else
				tree-&gt;op = LLSHIFT;
		}
		break;

	/*
	 * Try to change * and / to shifts.
	 */
	case TIMES:
	case DIVIDE:
	case ASTIMES:
	case ASDIV:
		tree = pow2(tree);
	}
	/*
	 * Try to find postfix ++ and -- operators that can be
	 * pulled out and done after the rest of the expression
	 */
	if (table!=cctab &amp;&amp; table!=cregtab &amp;&amp; recurf&lt;2
	 &amp;&amp; (opdope[tree-&gt;op]&amp;LEAF)==0) {
		if (r=delay(&amp;atree, table, reg)) {
			tree = atree;
			table = efftab;
			reg = r-1;
		}
	}
	/*
	 * Basically, try to reorder the computation
	 * so  reg = x+y  is done as  reg = x; reg =+ y
	 */
	if (recurf==0 &amp;&amp; reorder(&amp;atree, table, reg)) {
		if (table==cctab &amp;&amp; atree-&gt;op==NAME)
			return(reg);
	}
	tree = atree;
	if (table==efftab &amp;&amp; tree-&gt;op==NAME)
		return(reg);
	if ((r=cexpr(tree, table, reg))&gt;=0)
		return(r);
	if (table!=regtab)  {
		if((r=cexpr(tree, regtab, reg))&gt;=0) {
	fixup:
			modf = isfloat(tree);
			if (table==sptab || table==lsptab) {
				if (tree-&gt;type==LONG) {
					printf(&quot;mov\tr%d,-(sp)\n&quot;,r+1);
					nstack++;
				}
				printf(&quot;mov%c	r%d,%c(sp)\n&quot;, modf, r,
					table==sptab? &#39;-&#39;:0);
				nstack++;
			}
			if (table==cctab)
				printf(&quot;tst%c	r%d\n&quot;, modf, r);
			return(r);
		}
	}
	if (tree-&gt;op&gt;0 &amp;&amp; tree-&gt;op&lt;RFORCE &amp;&amp; opntab[tree-&gt;op])
		error(&quot;No code table for op: %s&quot;, opntab[tree-&gt;op]);
	else
		error(&quot;No code table for op %d&quot;, tree-&gt;op);
	return(reg);
}

/*
 * Try to compile the tree with the code table using
 * registers areg and up.  If successful,
 * return the register where the value actually ended up.
 * If unsuccessful, return -1.
 *
 * Most of the work is the macro-expansion of the
 * code table.
 */
cexpr(atree, table, areg)
struct tnode *atree;
struct table *table;
{
	int c, r;
	register struct tnode *p, *p1, *tree;
	struct table *ctable;
	struct tnode *p2;
	char *string;
	int reg, reg1, rreg, flag, opd;
	char *opt;

	tree = atree;
	reg = areg;
	p1 = tree-&gt;tr2;
	c = tree-&gt;op;
	opd = opdope[c];
	/*
	 * When the value of a relational or a logical expression is
	 * desired, more work must be done.
	 */
	if ((opd&amp;RELAT||c==LOGAND||c==LOGOR||c==EXCLA) &amp;&amp; table!=cctab) {
		cbranch(tree, c=isn++, 1, reg);
		rcexpr(&amp;czero, table, reg);
		branch(isn, 0);
		label(c);
		rcexpr(&amp;cone, table, reg);
		label(isn++);
		return(reg);
	}
	if(c==QUEST) {
		if (table==cctab)
			return(-1);
		cbranch(tree-&gt;tr1, c=isn++, 0, reg);
		flag = nstack;
		rreg = rcexpr(p1-&gt;tr1, table, reg);
		nstack = flag;
		branch(r=isn++, 0);
		label(c);
		reg = rcexpr(p1-&gt;tr2, table, rreg);
		if (rreg!=reg)
			printf(&quot;mov%c	r%d,r%d\n&quot;,
			    isfloat(tree),reg,rreg);
		label(r);
		return(rreg);
	}
	reg = oddreg(tree, reg);
	reg1 = reg+1;
	/*
	 * long values take 2 registers.
	 */
	if (tree-&gt;type==LONG &amp;&amp; tree-&gt;op!=ITOL)
		reg1++;
	/*
	 * Leaves of the expression tree
	 */
	if ((r = chkleaf(tree, table, reg)) &gt;= 0)
		return(r);
	/*
	 * x + (-1) is better done as x-1.
	 */

	if ((tree-&gt;op==PLUS||tree-&gt;op==ASPLUS) &amp;&amp;
	    (p1=tree-&gt;tr2)-&gt;op == CON &amp;&amp; p1-&gt;value == -1) {
		p1-&gt;value = 1;
		tree-&gt;op =+ (MINUS-PLUS);
	}
	if (table==cregtab)
		table = regtab;
	/*
	 * The following peculiar code depends on the fact that
	 * if you just want the codition codes set, efftab
	 * will generate the right code unless the operator is
	 * postfix ++ or --. Unravelled, if the table is
	 * cctab and the operator is not special, try first
	 * for efftab;  if the table isn&#39;t, if the operator is,
	 * or the first match fails, try to match
	 * with the table actually asked for.
	 */
	if (table!=cctab || c==INCAFT || c==DECAFT
	 || (opt = match(tree, efftab, nreg-reg)) == 0)
		if ((opt=match(tree, table, nreg-reg))==0)
			return(-1);
	string = opt-&gt;tabstring;
	p1 = tree-&gt;tr1;
	p2 = 0;
	if (opdope[tree-&gt;op]&amp;BINARY)
		p2 = tree-&gt;tr2;
loop:
	/*
	 * The 0200 bit asks for a tab.
	 */
	if ((c = *string++) &amp; 0200) {
		c =&amp; 0177;
		putchar(&#39;\t&#39;);
	}
	switch (c) {

	case &#39;\0&#39;:
		if (!isfloat(tree))
			if (tree-&gt;op==DIVIDE || tree-&gt;op==ASDIV)
				reg--;
		return(reg);

	/* A1 */
	case &#39;A&#39;:
		p = p1;
		goto adr;

	/* A2 */
	case &#39;B&#39;:
		p = p2;
		goto adr;

	adr:
		c = 0;
		if (*string==&#39;\&#39;&#39;) {
			c = 1;
			string++;
		} else if (*string==&#39;+&#39;) {
			c = 2;
			string++;
		}
		pname(p, c);
		goto loop;

	/* I */
	case &#39;M&#39;:
		if ((c = *string)==&#39;\&#39;&#39;)
			string++;
		else
			c = 0;
		prins(tree-&gt;op, c, instab);
		goto loop;

	/* B1 */
	case &#39;C&#39;:
		if ((opd&amp;LEAF) != 0)
			p = tree;
		else
			p = p1;
		goto pbyte;

	/* BF */
	case &#39;P&#39;:
		p = tree;
		goto pb1;

	/* B2 */
	case &#39;D&#39;:
		p = p2;
	pbyte:
		if (p-&gt;type==CHAR)
			putchar(&#39;b&#39;);
	pb1:
		if (isfloat(p))
			putchar(&#39;f&#39;);
		goto loop;

	/* BE */
	case &#39;L&#39;:
		if (p1-&gt;type==CHAR || p2-&gt;type==CHAR)
			putchar(&#39;b&#39;);
		p = tree;
		goto pb1;

	/* F */
	case &#39;G&#39;:
		p = p1;
		flag = 01;
		goto subtre;

	/* S */
	case &#39;K&#39;:
		p = p2;
		flag = 02;
		goto subtre;

	/* H */
	case &#39;H&#39;:
		p = tree;
		flag = 04;

	subtre:
		ctable = regtab;
		c = *string++ - &#39;A&#39;;
		if (*string==&#39;!&#39;) {
			string++;
			c =| 020;	/* force right register */
		}
		if ((c&amp;02)!=0)
			ctable = sptab;
		if ((c&amp;04)!=0)
			ctable = cctab;
		if ((flag&amp;01) &amp;&amp; ctable==regtab &amp;&amp; (c&amp;01)==0
		  &amp;&amp; (tree-&gt;op==DIVIDE||tree-&gt;op==MOD
		   || tree-&gt;op==ASDIV||tree-&gt;op==ASMOD))
			ctable = cregtab;
		if ((c&amp;01)!=0) {
			p = p-&gt;tr1;
			if(collcon(p) &amp;&amp; ctable!=sptab) {
				if (p-&gt;op==STAR)
					p = p-&gt;tr1;
				p = p-&gt;tr1;
			}
		}
		if (table==lsptab &amp;&amp; ctable==sptab)
			ctable = lsptab;
		if (c&amp;010)
			r = reg1;
		else
			if (opdope[p-&gt;op]&amp;LEAF || p-&gt;degree &lt; 2)
				r = reg;
			else
				r = areg;
		rreg = rcexpr(p, ctable, r);
		if (ctable!=regtab &amp;&amp; ctable!=cregtab)
			goto loop;
		if (c&amp;010) {
			if (c&amp;020 &amp;&amp; rreg!=reg1)
				printf(&quot;mov%c	r%d,r%d\n&quot;,
				    isfloat(tree),rreg,reg1);
			else
				reg1 = rreg;
		} else if (rreg!=reg)
			if ((c&amp;020)==0 &amp;&amp; oddreg(tree, 0)==0 &amp;&amp; (flag&amp;04 ||
			      flag&amp;01
			  &amp;&amp; xdcalc(p2, nreg-rreg-1) &lt;= (opt-&gt;tabdeg2&amp;077)
			 ||   flag&amp;02
			  &amp;&amp; xdcalc(p1,nreg-rreg-1) &lt;= (opt-&gt;tabdeg1&amp;077))) {
				reg = rreg;
				reg1 = rreg+1;
			} else
				printf(&quot;mov%c\tr%d,r%d\n&quot;,
				    isfloat(tree), rreg, reg);
		goto loop;

	/* R */
	case &#39;I&#39;:
		r = reg;
		if (*string==&#39;-&#39;) {
			string++;
			r--;
		}
		goto preg;

	/* R1 */
	case &#39;J&#39;:
		r = reg1;
	preg:
		if (*string==&#39;+&#39;) {
			string++;
			r++;
		}
		if (r&gt;nreg)
			error(&quot;Register overflow: simplify expression&quot;);
		printf(&quot;r%d&quot;, r);
		goto loop;

	case &#39;-&#39;:		/* check -(sp) */
		if (*string==&#39;(&#39;) {
			nstack++;
			if (table!=lsptab)
				putchar(&#39;-&#39;);
			goto loop;
		}
		break;

	case &#39;)&#39;:		/* check (sp)+ */
		putchar(&#39;)&#39;);
		if (*string==&#39;+&#39;)
			nstack--;
		goto loop;

	/* #1 */
	case &#39;#&#39;:
		p = p1-&gt;tr1;
		goto nmbr;

	/* #2 */
	case &#39;&quot;&#39;:
		p = p2-&gt;tr1;

	nmbr:
		if(collcon(p)) {
			if (p-&gt;op==STAR) {
				printf(&quot;*&quot;);
				p = p-&gt;tr1;
			}
			if ((p = p-&gt;tr2)-&gt;op == CON) {
				if (p-&gt;value)
					psoct(p-&gt;value);
			} else if (p-&gt;op==AMPER)
				pname(p-&gt;tr1, 0);
		}
		goto loop;

	case &#39;T&#39;:		/* &quot;tst R&quot; if 1st op not in cctab */
		if (dcalc(p1, 5)&gt;12 &amp;&amp; !match(p1, cctab, 10))
			printf(&quot;tst	r%d\n&quot;, reg);
		goto loop;
	case &#39;V&#39;:	/* adc or sbc as required for longs */
		switch(tree-&gt;op) {
		case PLUS:
		case ASPLUS:
		case INCBEF:
		case INCAFT:
			printf(&quot;adc&quot;);
			break;

		case MINUS:
		case ASMINUS:
		case NEG:
		case DECBEF:
		case DECAFT:
			printf(&quot;sbc&quot;);
			break;

		default:
			while ((c = *string++)!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39;);
			break;
		}
		goto loop;
	}
	putchar(c);
	goto loop;
}

/*
 * This routine just calls sreorder (below)
 * on the subtrees and then on the tree itself.
 * It returns non-zero if anything changed.
 */
reorder(treep, table, reg)
struct tnode **treep;
struct table *table;
{
	register r, r1;
	register struct tnode *p;

	p = *treep;
	if (opdope[p-&gt;op]&amp;LEAF)
		return(0);
	r1 = 0;
	while(sreorder(&amp;p-&gt;tr1, table, reg))
		r1++;
	if (opdope[p-&gt;op]&amp;BINARY) 
		while(sreorder(&amp;p-&gt;tr2, table, reg))
			r1++;
	r = 0;
	while (sreorder(treep, table, reg))
		r++;
	*treep = optim(*treep);
	return(r);
}

/*
 * Basically this routine carries out two kinds of optimization.
 * First, it observes that &quot;x + (reg = y)&quot; where actually
 * the = is any assignment op is better done as &quot;reg=y; x+reg&quot;.
 * In this case rcexpr is called to do the first part and the
 * tree is modified so the name of the register
 * replaces the assignment.
 * Moreover, expressions like &quot;reg = x+y&quot; are best done as
 * &quot;reg = x; reg =+ y&quot; (so long as &quot;reg&quot; and &quot;y&quot; are not the same!).
 */
sreorder(treep, table, reg)
struct tnode **treep;
struct table *table;
{
	register struct tnode *p, *p1;

	p = *treep;
	if (opdope[p-&gt;op]&amp;LEAF)
		return(0);
	if (p-&gt;op==PLUS)
		if (reorder(&amp;p-&gt;tr2, table, reg))
			*treep = p = optim(p);
	p1 = p-&gt;tr1;
	if (p-&gt;op==STAR || p-&gt;op==PLUS) {
		if (reorder(&amp;p-&gt;tr1, table, reg))
			*treep = p = optim(p);
		p1 = p-&gt;tr1;
	}
	if (p1-&gt;op==NAME) switch(p-&gt;op) {
		case ASLSH:
		case ASRSH:
		case ASSIGN:
			if (p1-&gt;class != REG || isfloat(p-&gt;tr2))
				return(0);
			if (p-&gt;op==ASSIGN) switch (p-&gt;tr2-&gt;op) {
			case TIMES:
			case DIVIDE:
				if (!ispow2(p-&gt;tr2))
					break;
				p-&gt;tr2 = pow2(p-&gt;tr2);
			case PLUS:
			case MINUS:
			case AND:
			case NAND:
			case OR:
			case EXOR:
			case LSHIFT:
			case RSHIFT:
				p1 = p-&gt;tr2-&gt;tr2;
				if (xdcalc(p1) &gt; 12
				 || p1-&gt;op==NAME
				 &amp;&amp;(p1-&gt;nloc==p-&gt;tr1-&gt;nloc
				  || p1-&gt;regno==p-&gt;tr1-&gt;nloc))
					return(0);
				p1 = p-&gt;tr2;
				p-&gt;tr2 = p1-&gt;tr1;
				if (p1-&gt;tr1-&gt;op!=NAME
				 || p1-&gt;tr1-&gt;class!=REG
				 || p1-&gt;tr1-&gt;nloc!=p-&gt;tr1-&gt;nloc)
					rcexpr(p, efftab, reg);
				p-&gt;tr2 = p1-&gt;tr2;
				p-&gt;op = p1-&gt;op + ASPLUS - PLUS;
				*treep = p;
				return(1);
			}
			goto OK;

		case ASTIMES:
		case ASDIV:
			if (!ispow2(p))
				return(0);
		case ASPLUS:
		case ASMINUS:
		case ASSAND:
		case ASSNAND:
		case ASOR:
		case ASXOR:
		case DECBEF:
		case INCBEF:
		OK:
			if (table==cctab||table==cregtab)
				reg =+ 020;
			rcexpr(optim(p), efftab, ~reg);
			*treep = p1;
			return(1);
	}
	return(0);
}

/*
 * Delay handles postfix ++ and -- 
 * It observes that &quot;x + y++&quot; is better
 * treated as &quot;x + y; y++&quot;.
 * If the operator is ++ or -- itself,
 * it calls rcexpr to load the operand, letting
 * the calling instance of rcexpr to do the
 * ++ using efftab.
 * Otherwise it uses sdelay to search for inc/dec
 * among the operands.
 */
delay(treep, table, reg)
struct tnode **treep;
{
	register struct tnode *p, *p1;
	register r;

	p = *treep;
	if (table!=efftab &amp;&amp; (p-&gt;op==INCAFT||p-&gt;op==DECAFT)
	 &amp;&amp; p-&gt;tr1-&gt;op==NAME) {
		return(1+rcexpr(p-&gt;tr1, table, reg));
	}
	p1 = 0;
	if (opdope[p-&gt;op]&amp;BINARY)
		p1 = sdelay(&amp;p-&gt;tr2);
	if (p1==0)
		p1 = sdelay(&amp;p-&gt;tr1);
	if (p1) {
		r = rcexpr(optim(p), table, reg);
		*treep = p1;
		return(r+1);
	}
	return(0);
}

sdelay(ap)
struct tnode **ap;
{
	register struct tnode *p, *p1;

	p = *ap;
	if ((p-&gt;op==INCAFT||p-&gt;op==DECAFT) &amp;&amp; p-&gt;tr1-&gt;op==NAME) {
		*ap = ncopy(p-&gt;tr1);
		return(p);
	}
	if (p-&gt;op==STAR || p-&gt;op==PLUS)
		if (p1=sdelay(&amp;p-&gt;tr1))
			return(p1);
	if (p-&gt;op==PLUS)
		return(sdelay(&amp;p-&gt;tr2));
	return(0);
}

/*
 * Copy a tree node for a register variable.
 * Used by sdelay because if *reg-- is turned
 * into *reg; reg-- the *reg will in turn
 * be changed to some offset class, accidentally
 * modifying the reg--.
 */
ncopy(ap)
struct tname *ap;
{
	register struct tname *p;

	p = ap;
	if (p-&gt;class!=REG)
		return(p);
	return(block(3, NAME, p-&gt;type, p-&gt;elsize, p-&gt;tr1,
	    p-&gt;offset, p-&gt;nloc));
}

/*
 * If the tree can be immediately loaded into a register,
 * produce code to do so and return success.
 */
chkleaf(atree, table, reg)
struct tnode *atree;
{
	struct tnode lbuf;
	register struct tnode *tree;

	tree = atree;
	if (tree-&gt;op!=STAR &amp;&amp; dcalc(tree, nreg-reg) &gt; 12)
		return(-1);
	lbuf.op = LOAD;
	lbuf.type = tree-&gt;type;
	lbuf.degree = tree-&gt;degree;
	lbuf.tr1 = tree;
	return(rcexpr(&amp;lbuf, table, reg));
}

/*
 * Compile a function argument.
 * If the stack is currently empty, put it in (sp)
 * rather than -(sp); this will save a pop.
 * Return the number of bytes pushed,
 * for future popping.
 */
comarg(atree, flagp)
int *flagp;
{
	register struct tnode *tree;
	register retval;

	tree = atree;
	if (nstack || isfloat(tree) || tree-&gt;type==LONG) {
		rcexpr(tree, sptab, 0);
		retval = arlength(tree-&gt;type);
	} else {
		(*flagp)++;
		rcexpr(tree, lsptab, 0);
		retval = 0;
	}
	return(retval);
}
