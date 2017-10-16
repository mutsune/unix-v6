#
/* C compiler
 *
 *
 *
 * Called from cc:
 *   c0 source temp1 temp2 [ profileflag ]
 * temp1 contains some ascii text and the binary expression
 * trees.  Each tree is introduced by the # character.
 * Strings are put on temp2, which cc tacks onto
 * temp1 for assembly.
 */

#include &quot;c0h.c&quot;

int	isn	1;
int	stflg	1;
int	peeksym	-1;
int	line	1;
int	debug	0;
int	dimp	0;
struct	tname	funcblk { NAME, 0, 0, REG, 0, 0 };
int	*treespace { osspace };

struct kwtab {
	char	*kwname;
	int	kwval;
} kwtab[]
{
	&quot;int&quot;,		INT,
	&quot;char&quot;,		CHAR,
	&quot;float&quot;,	FLOAT,
	&quot;double&quot;,	DOUBLE,
	&quot;struct&quot;,	STRUCT,
	&quot;long&quot;,		LONG,
	&quot;auto&quot;,		AUTO,
	&quot;extern&quot;,	EXTERN,
	&quot;static&quot;,	STATIC,
	&quot;register&quot;,	REG,
	&quot;goto&quot;,		GOTO,
	&quot;return&quot;,	RETURN,
	&quot;if&quot;,		IF,
	&quot;while&quot;,	WHILE,
	&quot;else&quot;,		ELSE,
	&quot;switch&quot;,	SWITCH,
	&quot;case&quot;,		CASE,
	&quot;break&quot;,	BREAK,
	&quot;continue&quot;,	CONTIN,
	&quot;do&quot;,		DO,
	&quot;default&quot;,	DEFAULT,
	&quot;for&quot;,		FOR,
	&quot;sizeof&quot;,	SIZEOF,
	0,		0,
};

main(argc, argv)
char *argv[];
{
	extern fin;
	register char *sp;
	register i;
	register struct kwtab *ip;

	if(argc&lt;3) {
		error(&quot;Arg count&quot;);
		exit(1);
	}
	if((fin=open(argv[1],0))&lt;0) {
		error(&quot;Can&#39;t find %s&quot;, argv[1]);
		exit(1);
	}
	if (fcreat(argv[2], obuf)&lt;0 || fcreat(argv[3], sbuf)&lt;0) {
		error(&quot;Can&#39;t create temp&quot;);
		exit(1);
	}
	if (argc&gt;4)
		proflg++;
	/*
	 * The hash table locations of the keywords
	 * are marked; if an identifier hashes to one of
	 * these locations, it is looked up in in the keyword
	 * table first.
	 */
	for (ip=kwtab; (sp = ip-&gt;kwname); ip++) {
		i = 0;
		while (*sp)
			i =+ *sp++;
		hshtab[i%hshsiz].hflag = FKEYW;
	}
	while(!eof) {
		extdef();
		blkend();
	}
	outcode(&quot;B&quot;, EOF);
	strflg++;
	outcode(&quot;B&quot;, EOF);
	fflush(obuf);
	fflush(sbuf);
	exit(nerror!=0);
}

/*
 * Look up the identifier in symbuf in the symbol table.
 * If it hashes to the same spot as a keyword, try the keyword table
 * first.  An initial &quot;.&quot; is ignored in the hash.
 * Return is a ptr to the symbol table entry.
 */
lookup()
{
	int ihash;
	register struct hshtab *rp;
	register char *sp, *np;

	ihash = 0;
	sp = symbuf;
	if (*sp==&#39;.&#39;)
		sp++;
	while (sp&lt;symbuf+ncps)
		ihash =+ *sp++;
	rp = &amp;hshtab[ihash%hshsiz];
	if (rp-&gt;hflag&amp;FKEYW)
		if (findkw())
			return(KEYW);
	while (*(np = rp-&gt;name)) {
		for (sp=symbuf; sp&lt;symbuf+ncps;)
			if (*np++ != *sp++)
				goto no;
		csym = rp;
		return(NAME);
	no:
		if (++rp &gt;= &amp;hshtab[hshsiz])
			rp = hshtab;
	}
	if(++hshused &gt;= hshsiz) {
		error(&quot;Symbol table overflow&quot;);
		exit(1);
	}
	rp-&gt;hclass = 0;
	rp-&gt;htype = 0;
	rp-&gt;hoffset = 0;
	rp-&gt;dimp = 0;
	rp-&gt;hflag =| xdflg;
	sp = symbuf;
	for (np=rp-&gt;name; sp&lt;symbuf+ncps;)
		*np++ = *sp++;
	csym = rp;
	return(NAME);
}

/*
 * Search the keyword table.
 * Ignore initial &quot;.&quot; to avoid member-of-structure
 * problems.
 */
findkw()
{
	register struct kwtab *kp;
	register char *p1, *p2;
	char *wp;

	wp = symbuf;
	if (*wp==&#39;.&#39;)
		wp++;
	for (kp=kwtab; (p2 = kp-&gt;kwname); kp++) {
		p1 = wp;
		while (*p1 == *p2++)
			if (*p1++ == &#39;\0&#39;) {
				cval = kp-&gt;kwval;
				return(1);
			}
	}
	return(0);
}


/*
 * Return the next symbol from the input.
 * peeksym is a pushed-back symbol, peekc is a pushed-back
 * character (after peeksym).
 * mosflg means that the next symbol, if an identifier,
 * is a member of structure or a structure tag, and it
 * gets a &quot;.&quot; prepended to it to distinguish
 * it from other identifiers.
 */
symbol() {
	register c;
	register char *sp;

	if (peeksym&gt;=0) {
		c = peeksym;
		peeksym = -1;
		if (c==NAME)
			mosflg = 0;
		return(c);
	}
	if (peekc) {
		c = peekc;
		peekc = 0;
	} else
		if (eof)
			return(EOF);
		else
			c = getchar();
loop:
	switch(ctab[c]) {

	case INSERT:		/* ignore newlines */
		inhdr = 1;
		c = getchar();
		goto loop;

	case NEWLN:
		if (!inhdr)
			line++;
		inhdr = 0;

	case SPACE:
		c = getchar();
		goto loop;

	case EOF:
		eof++;
		return(0);

	case PLUS:
		return(subseq(c,PLUS,INCBEF));

	case MINUS:
		return(subseq(c,subseq(&#39;&gt;&#39;,MINUS,ARROW),DECBEF));

	case ASSIGN:
		if (subseq(&#39; &#39;,0,1)) return(ASSIGN);
		c = symbol();
		if (c&gt;=PLUS &amp;&amp; c&lt;=EXOR) {
			if (spnextchar() != &#39; &#39;
			 &amp;&amp; (c==MINUS || c==AND || c==TIMES)) {
				error(&quot;Warning: assignment operator assumed&quot;);
				nerror--;
			}
			return(c+ASPLUS-PLUS);
		}
		if (c==ASSIGN)
			return(EQUAL);
		peeksym = c;
		return(ASSIGN);

	case LESS:
		if (subseq(c,0,1)) return(LSHIFT);
		return(subseq(&#39;=&#39;,LESS,LESSEQ));

	case GREAT:
		if (subseq(c,0,1)) return(RSHIFT);
		return(subseq(&#39;=&#39;,GREAT,GREATEQ));

	case EXCLA:
		return(subseq(&#39;=&#39;,EXCLA,NEQUAL));

	case DIVIDE:
		if (subseq(&#39;*&#39;,1,0))
			return(DIVIDE);
		while ((c = spnextchar()) != EOF) {
			peekc = 0;
			if (c==&#39;*&#39;) {
				if (spnextchar() == &#39;/&#39;) {
					peekc = 0;
					c = getchar();
					goto loop;
				}
			}
		}
		eof++;
			error(&quot;Nonterminated comment&quot;);
			return(0);

	case PERIOD:
	case DIGIT:
		peekc = c;
		if ((c=getnum(c==&#39;0&#39;?8:10)) == FCON)
			cval = isn++;
		return(c);

	case DQUOTE:
		return(getstr());

	case SQUOTE:
		return(getcc());

	case LETTER:
		sp = symbuf;
		if (mosflg) {
			*sp++ = &#39;.&#39;;
			mosflg = 0;
		}
		while(ctab[c]==LETTER || ctab[c]==DIGIT) {
			if (sp&lt;symbuf+ncps) *sp++ = c;
			c = getchar();
		}
		while(sp&lt;symbuf+ncps)
			*sp++ = &#39;\0&#39;;
		peekc = c;
		if ((c=lookup())==KEYW &amp;&amp; cval==SIZEOF)
			c = SIZEOF;
		return(c);

	case AND:
		return(subseq(&#39;&amp;&#39;, AND, LOGAND));

	case OR:
		return(subseq(&#39;|&#39;, OR, LOGOR));

	case UNKN:
		error(&quot;Unknown character&quot;);
		c = getchar();
		goto loop;

	}
	return(ctab[c]);
}

/*
 * If the next input character is c, return a and advance.
 * Otherwise push back the character and return a.
 */
subseq(c,a,b)
{
	if (spnextchar() != c)
		return(a);
	peekc = 0;
	return(b);
}

/*
 * Read a double-quoted string, placing it on the
 * string buffer.
 */
getstr()
{
	register int c;
	register char *sp;

	nchstr = 1;
	sp = savstr;
	while((c=mapch(&#39;&quot;&#39;)) &gt;= 0) {
		nchstr++;
		if (sp &gt;= &amp;savstr[STRSIZ]) {
			sp = savstr;
			error(&quot;String too long&quot;);
		}
		*sp++ = c;
	}
	strptr = sp;
	cval = isn++;
	return(STRING);
}

/*
 * Write out a string, either in-line
 * or in the string temp file labelled by
 * lab.
 */
putstr(lab)
{
	register char *sp;

	if (lab) {
		strflg++;
		outcode(&quot;BNB&quot;, LABEL, lab, BDATA);
	} else
		outcode(&quot;B&quot;, BDATA);
	for (sp = savstr; sp&lt;strptr; )
		outcode(&quot;1N&quot;, *sp++ &amp; 0377);
	outcode(&quot;100&quot;);
	strflg = 0;
}

/*
 * read a single-quoted character constant.
 * The routine is sensitive to the layout of
 * characters in a word.
 */
getcc()
{
	register int c, cc;
	register char *ccp;

	cval = 0;
	ccp = &amp;cval;
	cc = 0;
	while((c=mapch(&#39;\&#39;&#39;)) &gt;= 0)
		if(cc++ &lt; NCPW)
			*ccp++ = c;
	if(cc&gt;NCPW)
		error(&quot;Long character constant&quot;);
	return(CON);
}

/*
 * Read a character in a string or character constant,
 * detecting the end of the string.
 * It implements the escape sequences.
 */
mapch(ac)
{
	register int a, c, n;
	static mpeek;

	c = ac;
	if (a = mpeek)
		mpeek = 0;
	else
		a = getchar();
loop:
	if (a==c)
		return(-1);
	switch(a) {

	case &#39;\n&#39;:
	case &#39;\0&#39;:
		error(&quot;Nonterminated string&quot;);
		peekc = a;
		return(-1);

	case &#39;\\&#39;:
		switch (a=getchar()) {

		case &#39;t&#39;:
			return(&#39;\t&#39;);

		case &#39;n&#39;:
			return(&#39;\n&#39;);

		case &#39;b&#39;:
			return(&#39;\b&#39;);

		case &#39;0&#39;: case &#39;1&#39;: case &#39;2&#39;: case &#39;3&#39;:
		case &#39;4&#39;: case &#39;5&#39;: case &#39;6&#39;: case &#39;7&#39;:
			n = 0;
			c = 0;
			while (++c&lt;=3 &amp;&amp; &#39;0&#39;&lt;=a &amp;&amp; a&lt;=&#39;7&#39;) {
				n =&lt;&lt; 3;
				n =+ a-&#39;0&#39;;
				a = getchar();
			}
			mpeek = a;
			return(n);

		case &#39;r&#39;:
			return(&#39;\r&#39;);

		case &#39;\n&#39;:
			if (!inhdr)
				line++;
			inhdr = 0;
			a = getchar();
			goto loop;
		}
	}
	return(a);
}

/*
 * Read an expression and return a pointer to its tree.
 * It&#39;s the classical bottom-up, priority-driven scheme.
 * The initflg prevents the parse from going past
 * &quot;,&quot; or &quot;:&quot; because those delimitesrs are special
 * in initializer (and some other) expressions.
 */
tree()
{
#define	SEOF	200
#define	SSIZE	20
	int *op, opst[SSIZE], *pp, prst[SSIZE];
	register int andflg, o;
	register struct hshtab *cs;
	int p, ps, os;

	space = treespace;
	op = opst;
	pp = prst;
	cp = cmst;
	*op = SEOF;
	*pp = 06;
	andflg = 0;

advanc:
	switch (o=symbol()) {

	case NAME:
		cs = csym;
		if (cs-&gt;hclass==0 &amp;&amp; cs-&gt;htype==0)
			if(nextchar()==&#39;(&#39;) {
				/* set function */
				cs-&gt;hclass = EXTERN;
				cs-&gt;htype = FUNC;
			} else if (initflg)
				cs-&gt;hclass = EXTERN;
			else {
				/* set label */
				cs-&gt;htype = ARRAY;
				if (cs-&gt;hoffset==0)
					cs-&gt;hoffset = isn++;
			}
		*cp++ = copname(cs);
		goto tand;

	case FCON:
		if (!initflg)
			outcode(&quot;BBNB1N1N1N1N0B&quot;, DATA,LABEL,
			    cval, WDATA, fcval, PROG);

	case CON:
	case SFCON:
		*cp++ = block(1,o,(o==CON?INT:DOUBLE),0,cval);
		goto tand;

	/* fake a static char array */
	case STRING:
		putstr(cval);
		*cp++ = block(3, NAME, ARRAY+CHAR,0,STATIC,0,cval);

tand:
		if(cp&gt;=cmst+cmsiz) {
			error(&quot;Expression overflow&quot;);
			exit(1);
		}
		if (andflg)
			goto syntax;
		andflg = 1;
		goto advanc;

	case INCBEF:
	case DECBEF:
		if (andflg)
			o =+ 2;
		goto oponst;

	case COMPL:
	case EXCLA:
	case SIZEOF:
		if (andflg)
			goto syntax;
		goto oponst;

	case MINUS:
		if (!andflg)  {
			if ((peeksym=symbol())==FCON) {
				fcval = - fcval;
				goto advanc;
			}
			if (peeksym==SFCON) {
				fcval = - fcval;
				cval =^ 0100000;
				goto advanc;
			}
			o = NEG;
		}
		andflg = 0;
		goto oponst;

	case AND:
	case TIMES:
		if (andflg)
			andflg = 0; else
			if(o==AND)
				o = AMPER;
			else
				o = STAR;
		goto oponst;

	case LPARN:
		if (andflg) {
			o = symbol();
			if (o==RPARN)
				o = MCALL;
			else {
				peeksym = o;
				o = CALL;
				andflg = 0;
			}
		}
		goto oponst;

	case RBRACK:
	case RPARN:
		if (!andflg)
			goto syntax;
		goto oponst;

	case DOT:
	case ARROW:
		mosflg++;
		break;

	}
	/* binaries */
	if (!andflg)
		goto syntax;
	andflg = 0;

oponst:
	p = (opdope[o]&gt;&gt;9) &amp; 077;
	if ((o==COMMA || o==COLON) &amp;&amp; initflg)
		p = 05;
opon1:
	ps = *pp;
	if (p&gt;ps || p==ps &amp;&amp; (opdope[o]&amp;RASSOC)!=0) {
		switch (o) {

		case INCAFT:
		case DECAFT:
			p = 37;
			break;
		case LPARN:
		case LBRACK:
		case CALL:
			p = 04;
		}
		if (op &gt;= &amp;opst[SSIZE-1]) {
			error(&quot;expression overflow&quot;);
			exit(1);
		}
		*++op = o;
		*++pp = p;
		goto advanc;
	}
	--pp;
	switch (os = *op--) {

	case SEOF:
		peeksym = o;
		build(0);		/* flush conversions */
		return(*--cp);

	case CALL:
		if (o!=RPARN)
			goto syntax;
		build(os);
		goto advanc;

	case MCALL:
		*cp++ = block(0,0,0,0);	/* 0 arg call */
		os = CALL;
		break;

	case INCBEF:
	case INCAFT:
	case DECBEF:
	case DECAFT:
		*cp++ = block(1, CON, INT, 0, 1);
		break;

	case LPARN:
		if (o!=RPARN)
			goto syntax;
		goto advanc;

	case LBRACK:
		if (o!=RBRACK)
			goto syntax;
		build(LBRACK);
		goto advanc;
	}
	build(os);
	goto opon1;

syntax:
	error(&quot;Expression syntax&quot;);
	errflush(o);
	return(0);
}

/*
 * Generate a tree node for a name.
 * All the relevant info from the symbol table is
 * copied out, including the name if it&#39;s an external.
 * This is because the symbol table is gone in the next
 * pass, so a ptr isn&#39;t sufficient.
 */
copname(acs)
struct hshtab *acs;
{
	register struct hshtab *cs;
	register struct tname *tp;
	register char *cp1;
	int i;
	char *cp2;

	cs = acs;
	tp = gblock(sizeof(*tp)/NCPW);
	tp-&gt;op = NAME;
	tp-&gt;type = cs-&gt;htype;
	tp-&gt;dimp = cs-&gt;hdimp;
	if ((tp-&gt;class = cs-&gt;hclass)==0)
		tp-&gt;class = STATIC;
	tp-&gt;offset = 0;
	tp-&gt;nloc = cs-&gt;hoffset;
	if (cs-&gt;hclass==EXTERN) {
		gblock((ncps-NCPW)/NCPW);
		cp1 = tp-&gt;nname;
		cp2 = cs-&gt;name;
		i = ncps;
		do {
			*cp1++ = *cp2++;
		} while (--i);
	}
	if (cs-&gt;hflag&amp;FFIELD)
		tp-&gt;class = FMOS;
	return(tp);
}
