int	nofloat;
int	peekc;
int	obuf[259];
int	tabflg;
int	labno	1;

main(argc, argv)
char **argv;
{
/*
	A1 -&gt; A
	A2    B
	A     O
	B1    C
	B2    D
	BE    L
	BF    P
	C1    E
	C2    F
	F     G
	H     H
	R     I
	R1    J
	S     K
	I     M
	M     N

		*	+1
		S	+2
		C	+4
		1	+8

	z  -&gt; 4
	c     10
	a     14
	e     20
	n     63
	*	+0100
*/

	auto c,snlflg,nlflg,t,smode,m,ssmode;
	extern fin;

	smode = nlflg = snlflg = ssmode = 0;
	if (argc&gt;1)
		if ((fin = open(argv[1], 0)) &lt; 0) {
			putchar(&#39;?\n&#39;);
			return;
		}
	obuf[0] = 1;
	if (argc&gt;2) 
		if ((obuf[0] = creat(argv[2], 0666)) &lt; 0) {
			putchar(&#39;?\n&#39;);
			return;
		}
loop:
	c = getc();
	if (c!=&#39;\n&#39; &amp;&amp; c!=&#39;\t&#39;) nlflg = 0;
	if (ssmode!=0 &amp;&amp; c!=&#39;%&#39;) {
		ssmode = 0;
		printf(&quot;.data\nL%d:&lt;&quot;, labno++);
	}
	switch(c) {

	case &#39;\0&#39;:
		printf(&quot;.text; 0\n&quot;);
		fflush(obuf);
		return;

	case &#39;:&#39;:
		if (!smode)
			printf(&quot;=.+2; 0&quot;); else
			putchar(&#39;:&#39;);
		goto loop;

	case &#39;A&#39;:
		if ((c=getc())==&#39;1&#39; || c==&#39;2&#39;) {
			putchar(c+&#39;A&#39;-&#39;1&#39;);
			goto loop;
		}
		putchar(&#39;O&#39;);
		peekc = c;
		goto loop;

	case &#39;B&#39;:
		switch (getc()) {

		case &#39;1&#39;:
			putchar(&#39;C&#39;);
			goto loop;

		case &#39;2&#39;:
			putchar(&#39;D&#39;);
			goto loop;

		case &#39;E&#39;:
			putchar(&#39;L&#39;);
			goto loop;

		case &#39;F&#39;:
			putchar(&#39;P&#39;);
			goto loop;
		}
		putchar(&#39;?&#39;);
		goto loop;

	case &#39;C&#39;:
		putchar(getc()+&#39;E&#39;-&#39;1&#39;);
		goto loop;

	case &#39;F&#39;:
		putchar(&#39;G&#39;);
		goto subtre;

	case &#39;R&#39;:
		if ((c=getc()) == &#39;1&#39;)
		putchar(&#39;J&#39;); else {
			putchar(&#39;I&#39;);
			peekc = c;
		}
		goto loop;

	case &#39;H&#39;:
		putchar(&#39;H&#39;);
		goto subtre;

	case &#39;I&#39;:
		putchar(&#39;M&#39;);
		goto loop;

	case &#39;S&#39;:
		putchar(&#39;K&#39;);
subtre:
		snlflg = 1;
		t = &#39;A&#39;;
l1:
		switch (c=getc()) {

		case &#39;*&#39;:
			t++;
			goto l1;

		case &#39;S&#39;:
			t =+ 2;
			goto l1;

		case &#39;C&#39;:
			t =+ 4;
			goto l1;

		case &#39;1&#39;:
			t =+ 8;
			goto l1;

		case &#39;2&#39;:
			t =+ 16;
			goto l1;
		}
		peekc = c;
		putchar(t);
		goto loop;

	case &#39;#&#39;:
		if(getc()==&#39;1&#39;)
			putchar(&#39;#&#39;); else
			putchar(&#39;&quot;&#39;);
		goto loop;

	case &#39;%&#39;:
		if (smode)
			printf(&quot;.text;&quot;);
		if (ssmode==0) {
			if ((peekc=getc())==&#39;[&#39;) {
				peekc = 0;
				printf(&quot;.data;&quot;);
				while((c=getc())!=&#39;]&#39;)
					putchar(c);
				getc();
				printf(&quot;;.text;&quot;);
				goto loop;
			}
		}
loop1:
		switch (c=getc()) {

		case &#39; &#39;:
		case &#39;\t&#39;:
			goto loop1;
		case &#39;a&#39;:
			m = 16;
			t = flag();
			goto pf;

		case &#39;,&#39;:
			putchar(&#39;;&#39;);
			goto loop1;

		case &#39;i&#39;:
			m = 12;
			t = flag();
			goto pf;
		case &#39;z&#39;:
			m = 4;
			t = flag();
			goto pf;

		case &#39;r&#39;:
			m = 9;
			t = flag();
			goto pf;

		case &#39;1&#39;:
			m = 5;
			t = flag();
			goto pf;

		case &#39;c&#39;:
			t = 0;
			m = 8;
			goto pf;

		case &#39;e&#39;:
			t = flag();
			m = 20;
			goto pf;

		case &#39;n&#39;:
			t = flag();
			m = 63;
pf:
			if ((c=getc())==&#39;*&#39;)
				m =+ 0100; else
				peekc = c;
			printf(&quot;.byte %o,%o&quot;, m, t);
			goto loop1;
		case &#39;[&#39;:
			printf(&quot;L%d=&quot;, labno++);
			while ((c=getc())!=&#39;]&#39;)
				putchar(c);
			ssmode = 0;
			smode = 0;
			goto loop;

		case &#39;\n&#39;:
			printf(&quot;\nL%d\n&quot;, labno);
			ssmode = 1;
			nlflg = 1;
			smode = 1;
			goto loop;
		}
		putchar(c);
		goto loop1;

	case &#39;\t&#39;:
		if (nlflg) {
			nlflg = 0;
			goto loop;
		}
		if (smode) {
			tabflg++;
			goto loop;
		}
		putchar(&#39;\t&#39;);
		goto loop;

	case &#39;\n&#39;:
		if (!smode)  {
			putchar(&#39;\n&#39;);
			goto loop;
		}
		if (nlflg) {
			nlflg = 0;
			printf(&quot;\\0&gt;\n.text\n&quot;);
			smode = 0;
			goto loop;
		}
		if (!snlflg)
			printf(&quot;\\n&quot;);
		snlflg = 0;
		printf(&quot;&gt;\n&lt;&quot;);
		nlflg = 1;
		goto loop;

	case &#39;X&#39;:
	case &#39;Y&#39;:
	case &#39;T&#39;:
		snlflg++;
	}
	putchar(c);
	goto loop;
}

getc() {
	auto t, ifcnt;

	ifcnt = 0;
gc:
	if (peekc) {
		t = peekc;
		peekc = 0;
	} else
		t = getchar();
	if (t==0)
		return(0);
	if (t==&#39;{&#39;) {
		ifcnt++;
		t = getchar();
	}
	if (t==&#39;}&#39;) {
		t = getc();
		if (--ifcnt==0)
			if (t==&#39;\n&#39;)
				t = getc();
	}
	if (ifcnt &amp;&amp; nofloat)
		goto gc;
	return(t);
}

flag() {
	register c, f;

	f = 0;
l1:
	switch(c=getc()) {

	case &#39;w&#39;:
		f = 1;
		goto l1;

	case &#39;i&#39;:
		f = 2;
		goto l1;

	case &#39;b&#39;:
		f = 3;
		goto l1;

	case &#39;f&#39;:
		f = 4;
		goto l1;

	case &#39;d&#39;:
		f = 5;
		goto l1;

	case &#39;s&#39;:
		f = 6;
		goto l1;

	case &#39;l&#39;:
		f = 8;
		goto l1;

	case &#39;p&#39;:
		f =+ 16;
		goto l1;
	}
	peekc = c;
	return(f);
}

putchar(c)
{
	if (tabflg) {
		tabflg = 0;
		printf(&quot;&gt;;.byte %o;&lt;&quot;, c+0200);
	} else
		putc(c, obuf);
}
