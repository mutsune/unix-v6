#
/*
 * C compiler
 *
 *
 */

#include &quot;c0h.c&quot;

/*
 * Reduce the degree-of-reference by one.
 * e.g. turn &quot;ptr-to-int&quot; into &quot;int&quot;.
 */
decref(at)
{
	register t;

	t = at;
	if ((t &amp; ~TYPE) == 0) {
		error(&quot;Illegal indirection&quot;);
		return(t);
	}
	return((t&gt;&gt;TYLEN) &amp; ~TYPE | t&amp;TYPE);
}

/*
 * Increase the degree of reference by
 * one; e.g. turn &quot;int&quot; to &quot;ptr-to-int&quot;.
 */
incref(t)
{
	return(((t&amp;~TYPE)&lt;&lt;TYLEN) | (t&amp;TYPE) | PTR);
}

/*
 * Make a tree that causes a branch to lbl
 * if the tree&#39;s value is non-zero together with the cond.
 */
cbranch(tree, lbl, cond)
struct tnode *tree;
{
	rcexpr(block(1,CBRANCH,tree,lbl,cond));
}

/*
 * Write out a tree.
 */
rcexpr(tree)
struct tnode *tree;
{

	treeout(tree);
	outcode(&quot;BN&quot;, EXPR, line);
}

treeout(atree)
struct tnode *atree;
{
	register struct tnode *tree;

	if ((tree = atree) == 0)
		return;
	switch(tree-&gt;op) {

	case 0:
		outcode(&quot;B&quot;, NULL);
		return;

	case NAME:
		outcode(&quot;BNN&quot;, NAME, tree-&gt;class, tree-&gt;type);
		if (tree-&gt;class==EXTERN)
			outcode(&quot;S&quot;, tree-&gt;nname);
		else
			outcode(&quot;N&quot;, tree-&gt;nloc);
		return;

	case CON:
	case FCON:
	case SFCON:
		outcode(&quot;BNN&quot;, tree-&gt;op, tree-&gt;type, tree-&gt;value);
		return;

	case FSEL:
		treeout(tree-&gt;tr1);
		outcode(&quot;BNN&quot;, tree-&gt;op, tree-&gt;type, tree-&gt;tr2);
		return;

	case CBRANCH:
		treeout(tree-&gt;btree);
		outcode(&quot;BNN&quot;, tree-&gt;op, tree-&gt;lbl, tree-&gt;cond);
		return;

	default:
		treeout(tree-&gt;tr1);
		if (opdope[tree-&gt;op]&amp;BINARY)
			treeout(tree-&gt;tr2);
		outcode(&quot;BN&quot;, tree-&gt;op, tree-&gt;type);
		return;
	}
}

/*
 * Generate a branch
 */
branch(lab) {
	outcode(&quot;BN&quot;, BRANCH, lab);
}

/*
 * Generate a label
 */
label(l) {
	outcode(&quot;BN&quot;, LABEL, l);
}

/*
 * ap is a tree node whose type
 * is some kind of pointer; return the size of the object
 * to which the pointer points.
 */
plength(ap)
struct tname *ap;
{
	register t, l;
	register struct tname *p;

	p = ap;
	if (p==0 || ((t=p-&gt;type)&amp;~TYPE) == 0)		/* not a reference */
		return(1);
	p-&gt;type = decref(t);
	l = length(p);
	p-&gt;type = t;
	return(l);
}

/*
 * return the number of bytes in the object
 * whose tree node is acs.
 */
length(acs)
struct tnode *acs;
{
	register t, n;
	register struct tnode *cs;

	cs = acs;
	t = cs-&gt;type;
	n = 1;
	while ((t&amp;XTYPE) == ARRAY) {
		t = decref(t);
		n = dimtab[cs-&gt;ssp&amp;0377];
	}
	if ((t&amp;~TYPE)==FUNC)
		return(0);
	if (t&gt;=PTR)
		return(2*n);
	switch(t&amp;TYPE) {

	case INT:
		return(2*n);

	case CHAR:
		return(n);

	case FLOAT:
	case LONG:
		return(4*n);

	case DOUBLE:
		return(8*n);

	case STRUCT:
		return(n * dimtab[cs-&gt;lenp&amp;0377]);

	case RSTRUCT:
		error(&quot;Bad structure&quot;);
		return(0);
	}
	error(&quot;Compiler error (length)&quot;);
}

/*
 * The number of bytes in an object, rounded up to a word.
 */
rlength(cs)
struct tnode *cs;
{
	return((length(cs)+ALIGN) &amp; ~ALIGN);
}

/*
 * After an &quot;if (...) goto&quot;, look to see if the transfer
 * is to a simple label.
 */
simplegoto()
{
	register struct hshtab *csp;

	if ((peeksym=symbol())==NAME &amp;&amp; nextchar()==&#39;;&#39;) {
		csp = csym;
		if (csp-&gt;hclass==0 &amp;&amp; csp-&gt;htype==0) {
			csp-&gt;htype = ARRAY;
			if (csp-&gt;hoffset==0)
				csp-&gt;hoffset = isn++;
		}
		if ((csp-&gt;hclass==0||csp-&gt;hclass==STATIC)
		 &amp;&amp;  csp-&gt;htype==ARRAY) {
			peeksym = -1;
			return(csp-&gt;hoffset);
		}
	}
	return(0);
}

/*
 * Return the next non-white-space character
 */
nextchar()
{
	while (spnextchar()==&#39; &#39;)
		peekc = 0;
	return(peekc);
}

/*
 * Return the next character, translating all white space
 * to blank and handling line-ends.
 */
spnextchar()
{
	register c;

	if ((c = peekc)==0)
		c = getchar();
	if (c==&#39;\t&#39;)
		c = &#39; &#39;;
	else if (c==&#39;\n&#39;) {
		c = &#39; &#39;;
		if (inhdr==0)
			line++;
		inhdr = 0;
	} else if (c==&#39;\001&#39;) {	/* SOH, insert marker */
		inhdr++;
		c = &#39; &#39;;
	}
	peekc = c;
	return(c);
}

/*
 * is a break or continue legal?
 */
chconbrk(l)
{
	if (l==0)
		error(&quot;Break/continue error&quot;);
}

/*
 * The goto statement.
 */
dogoto()
{
	register struct tnode *np;

	*cp++ = tree();
	build(STAR);
	chkw(np = *--cp, -1);
	rcexpr(block(1,JUMP,0,0,np));
}

/*
 * The return statement, which has to convert
 * the returned object to the function&#39;s type.
 */
doret()
{
	register struct tnode *t;

	if (nextchar() != &#39;;&#39;) {
		t = tree();
		*cp++ = &amp;funcblk;
		*cp++ = t;
		build(ASSIGN);
		cp[-1] = cp[-1]-&gt;tr2;
		build(RFORCE);
		rcexpr(*--cp);
	}
	branch(retlab);
}

/*
 * write out a character to the usual output
 * or to the string file
 */
putchar(c)
{
	write(1, &amp;c, 1);
}

outcode(s, a)
char *s;
{
	register char *sp;
	register *ap, *bufp;
	int n;
	char *np;

	bufp = obuf;
	if (strflg)
		bufp = sbuf;
	ap = &amp;a;
	for (;;) switch(*s++) {
	case &#39;B&#39;:
		putw(*ap++ | (0376&lt;&lt;8), bufp);
		continue;

	case &#39;N&#39;:
		putw(*ap++, bufp);
		continue;

	case &#39;S&#39;:
		np = *ap++;
		n = ncps;
		while (n-- &amp;&amp; *np) {
			putc(*np++, bufp);
		}
		putc(0, bufp);
		continue;

	case &#39;1&#39;:
		putw(1, bufp);
		continue;

	case &#39;0&#39;:
		putw(0, bufp);
		continue;

	case &#39;\0&#39;:
		return;
	}
	error(&quot;Botch in outcode&quot;);
}
