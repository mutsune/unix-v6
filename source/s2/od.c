int	word[16];
char	gbuf[512];
int	fi;
int	conv;
int	base	010;
int	basem	01000;
int	max;
int	gidx;
int	gcnt;
int	eof;
int	addr[2];
int	from[2];
int	key;
int	flag;
int	nword	8;

main(argc, argv)
char **argv;
{
	extern fout;
	int f, k, w, i, a[2];
	char *p;

	fout = dup(1);
	argv++;
	if(argc &gt; 1) {
		p = *argv;
		if(*p == &#39;-&#39;) {
			while((i = *p++) != &#39;\0&#39;) {
				switch(i) {
				case &#39;o&#39;:
					conv =| 001;
					f = 6;
					break;
				case &#39;d&#39;:
					conv =| 002;
					f = 5;
					break;
				case &#39;a&#39;:
					conv =| 004;
					f = 4;
					break;
				case &#39;h&#39;:
					conv =| 010;
					f = 4;
					break;
				case &#39;c&#39;:
					conv =| 020;
					f = 5;
					break;
				case &#39;b&#39;:
					conv =| 040;
					f = 7;
					break;
				}
				if(f &gt; max)
					max = f;
			}
			argc--;
			argv++;
		}
	}
	if(!conv) {
		max = 6;
		conv = 1;
	}
	if(argc &gt; 1)
	if(**argv != &#39;+&#39;) {
		fi = open(*argv, 0);
		if(fi &lt; 0) {
			printf(&quot;cannot open %s\n&quot;, *argv);
			goto done;
		}
		argv++;
		argc--;
	}
	if(argc &gt; 1)
		offset(*argv);

loop:
	f = 1;
	a[0] = addr[0];
	a[1] = addr[1];
	for(i=0; i&lt;nword; i++) {
		w = getw();
		if(eof)
			break;
		word[i] = w;
		if(i)
			f =&amp; w==k; else
			k = w;
	}
	if(i)
	if(f &amp;&amp; !eof) {
		if(!(flag &amp;&amp; k==key)) {
			dupl();
			key = k;
			from[0] = a[0];
			from[1] = a[1];
		}
		flag =+ i;
	} else {
		dupl();
		line(a, word, i);
	}
	if(!eof)
		goto loop;
	puta(addr);
	putchar(&#39;\n&#39;);

done:
	flush();
}

dupl()
{

	if(flag) {
		flag = 0;
		line(from, &amp;key, 1);
	}
}

puta(a)
int a[2];
{

	putn(a[0], base, 4);
	putn(a[1], base, 3);
	putchar(&#39; &#39;);
}

line(a, w, n)
int w[];
{
	int i, f, c;

	f = 1;
	for(c=1; c; c=+c) {
		if((c&amp;conv) == 0)
			continue;
		if(f) {
			puta(a);
			f = 0;
		} else
			for(i=0; i&lt;8; i++)
				putchar(&#39; &#39;);
		for(i=0; i&lt;n; i++) {
			putx(w[i], c);
			putchar(i==n-1? &#39;\n&#39;: &#39; &#39;);
		}
	}
}

putx(n, c)
{

	switch(c) {
	case 001:
		pre(6);
		putn(n, 8, 6);
		break;
	case 002:
		pre(5);
		putn(n, 10, 5);
		break;
	case 004:
		pre(4);
		putop(n);
		break;
	case 010:
		pre(4);
		putn(n, 16, 4);
		break;
	case 020:
		pre(5);
		putc(n);
		putchar(&#39; &#39;);
		putc(n&gt;&gt;8);
		break;
	case 040:
		pre(7);
		putn(n&amp;0377, 8, 3);
		putchar(&#39; &#39;);
		putn((n&gt;&gt;8)&amp;0377, 8, 3);
		break;
	}
}

getw()
{
	int b1, b2;

	b1 = getc();
	if(b1 == -1) {
		eof = 1;
		return(0);
	}
	b2 = getc();
	if(b2 == -1)
		b2 = 0;
	return(b1|(b2&lt;&lt;8));
}

getc()
{

	if(gidx &gt;= gcnt) {
		gcnt = read(fi, gbuf, 512);
		if(gcnt &lt;= 0)
			return(-1);
		gidx = 0;
	}
	if(++addr[1] &gt;= basem) {
		addr[0]++;
		addr[1] = 0;
	}
	return(gbuf[gidx++]&amp;0377);
}

putc(c)
{
	c =&amp; 0377;
	if(c&gt;037 &amp;&amp; c&lt;0177 &amp;&amp; c!=&#39;\\&#39;) {
		putchar(&#39; &#39;);
		putchar(c);
		return;
	}
	putchar(&#39;\\&#39;);
	switch(c) {
	case &#39;\0&#39;:
		putchar(&#39;0&#39;);
		break;
	case &#39;\n&#39;:
		putchar(&#39;n&#39;);
		break;
	case &#39;\\&#39;:
		putchar(&#39;\\&#39;);
		break;
	case &#39;\t&#39;:
		putchar(&#39;t&#39;);
		break;
	default:
		putchar(&#39;?&#39;);
	}
}

putn(n, b, c)
{

	if(!c)
		return;
	putn(ldiv(0,n,b),b,c-1);
	if((n=lrem(0,n,b)) &gt; 9)
		putchar(n-10+&#39;a&#39;); else
		putchar(n+&#39;0&#39;);
}

pre(n)
{
	int i;

	for(i=n; i&lt;max; i++)
		putchar(&#39; &#39;);
}

offset(s)
char s[];
{
	char *p;
	int a[2], d, i, j, b;

	p = s;
	while(*p++);
	i = p-s-1;
	b = 0;
	if(i &gt; 0)
		if(p[-2] == &#39;b&#39;) {
			i--;
			b++;
			p--;
		}
	if(i &gt; 0)
		if(p[-2] == &#39;.&#39;) {
			i--;
			base = 10;
			basem = 1000;
		}
	a[0] = 0;
	for(j=0; i-j&gt;3; j++) {
		d = s[j];
		if(d&gt;=&#39;0&#39; &amp;&amp; d&lt;=&#39;9&#39;)
			a[0] = a[0]*base + d-&#39;0&#39;;
	}
	a[1] = 0;
	for(; i-j&gt;0; j++) {
		d = s[j];
		if(d&gt;=&#39;0&#39; &amp;&amp; d&lt;=&#39;9&#39;)
			a[1] = a[1]*base + d-&#39;0&#39;;
	}
	if(b) {
		i = a[0]*basem+a[1];
		a[0] = 0;
		a[1] = 0;
		while(i--) {
			a[1] =+ 512;
			while(a[1] &gt;= basem) {
				a[1] =- basem;
				a[0]++;
			}
		}
	}
	i = 0;
	while(a[0] &gt; addr[0]+1) {
		addr[1] =+ 512;
		while(addr[1] &gt;= basem) {
			addr[1] =- basem;
			addr[0]++;
		}
		i++;
	}
	seek(fi, i, 3);
	while(a[0]!=addr[0] || a[1]!=addr[1])
		if(getc() == -1)
			break;
}

putop(n)
{
	char *p;
	int i, c;

	p = getop(n);
	for(i=0; (c = *p++) != &#39;\0&#39;; i++)
		putchar(c);
	for(; i&lt;4; i++)
		putchar(&#39; &#39;);
}

getop(n)
{

	switch(n&amp;0170000)
	{
	case 0000000:
		switch(n&amp;0177000)
		{
		case 0004000:
			return(&quot;jsr&quot;);

		case 0077000:
			return(&quot;sob&quot;);
		}
		switch(n&amp;0177400)
		{
		case 0000400:
			return(&quot;br&quot;);

		case 0001000:
			return(&quot;bne&quot;);

		case 0001400:
			return(&quot;beq&quot;);

		case 0002000:
			return(&quot;bge&quot;);

		case 0002400:
			return(&quot;blt&quot;);

		case 0003000:
			return(&quot;bgt&quot;);

		case 0003400:
			return(&quot;ble&quot;);
		}
		switch(n&amp;0177700)
		{
		case 0000100:
			return(&quot;jmp&quot;);

		case 0000300:
			return(&quot;swab&quot;);

		case 0005000:
			return(&quot;clr&quot;);

		case 0005100:
			return(&quot;com&quot;);

		case 0005200:
			return(&quot;inc&quot;);

		case 0005300:
			return(&quot;dec&quot;);

		case 0005400:
			return(&quot;neg&quot;);

		case 0005500:
			return(&quot;adc&quot;);

		case 0005600:
			return(&quot;sbc&quot;);

		case 0005700:
			return(&quot;tst&quot;);

		case 0006000:
			return(&quot;ror&quot;);

		case 0006100:
			return(&quot;rol&quot;);

		case 0006200:
			return(&quot;asr&quot;);

		case 0006300:
			return(&quot;asl&quot;);

		case 0006400:
			return(&quot;mark&quot;);

		case 0006500:
			return(&quot;mfpi&quot;);

		case 0006600:
			return(&quot;mtpi&quot;);

		case 0006700:
			return(&quot;sxt&quot;);
		}
		switch(n&amp;0177740)
		{
		case 0000240:
			return(&quot;flag&quot;);
		}
		switch(n&amp;0177770)
		{
		case 0000200:
			return(&quot;rts&quot;);

		case 0000230:
			return(&quot;spl&quot;);
		}
		switch(n&amp;0177777)
		{
		case 0000000:
			return(&quot;halt&quot;);

		case 0000001:
			return(&quot;wait&quot;);

		case 0000002:
			return(&quot;rti&quot;);

		case 0000003:
			return(&quot;bpt&quot;);

		case 0000004:
			return(&quot;iot&quot;);

		case 0000005:
			return(&quot;rset&quot;);

		case 0000006:
			return(&quot;rtt&quot;);
		}
		break;

	case 0010000:
		return(&quot;mov &quot;);

	case 0020000:
		return(&quot;cmp&quot;);

	case 0030000:
		return(&quot;bit&quot;);

	case 0040000:
		return(&quot;bic&quot;);

	case 0050000:
		return(&quot;bis&quot;);

	case 0060000:
		return(&quot;add&quot;);

	case 0070000:
		switch(n&amp;0177000)
		{
		case 0070000:
			return(&quot;mul&quot;);

		case 0071000:
			return(&quot;div&quot;);

		case 0072000:
			return(&quot;ash&quot;);

		case 0073000:
			return(&quot;ashc&quot;);

		case 0074000:
			return(&quot;xor&quot;);
		}
		break;

	case 0100000:
		switch(n&amp;0177400)
		{
		case 0100000:
			return(&quot;bpl&quot;);

		case 0100400:
			return(&quot;bmi&quot;);

		case 0101000:
			return(&quot;bhi&quot;);

		case 0101400:
			return(&quot;blos&quot;);

		case 0102000:
			return(&quot;bvc&quot;);

		case 0102400:
			return(&quot;bvs&quot;);

		case 0103000:
			return(&quot;bhis&quot;);

		case 0103400:
			return(&quot;blo&quot;);

		case 0104000:
			return(&quot;emt&quot;);

		case 0104400:
			return(&quot;sys&quot;);
		}
		switch(n&amp;0177700)
		{
		case 0105000:
			return(&quot;clrb&quot;);

		case 0105100:
			return(&quot;comb&quot;);

		case 0105200:
			return(&quot;incb&quot;);

		case 0105300:
			return(&quot;decb&quot;);

		case 0105400:
			return(&quot;negb&quot;);

		case 0105500:
			return(&quot;adcb&quot;);

		case 0105600:
			return(&quot;sbcb&quot;);

		case 0105700:
			return(&quot;tstb&quot;);

		case 0106000:
			return(&quot;rorb&quot;);

		case 0106100:
			return(&quot;rolb&quot;);

		case 0106200:
			return(&quot;asrb&quot;);

		case 0106300:
			return(&quot;aslb&quot;);

		case 0106500:
			return(&quot;mfpd&quot;);

		case 0106600:
			return(&quot;mfpd&quot;);
		}
		break;

	case 0110000:
		return(&quot;movb&quot;);

	case 0120000:
		return(&quot;cmpb&quot;);

	case 0130000:
		return(&quot;bitb&quot;);

	case 0140000:
		return(&quot;bicb&quot;);

	case 0150000:
		return(&quot;bisb&quot;);

	case 0160000:
		return(&quot;sub&quot;);

	case 0170000:
		switch(n&amp;01777000)
		{
		case 0:0;
		}
		break;
	}
	return(&quot;???&quot;);
}
