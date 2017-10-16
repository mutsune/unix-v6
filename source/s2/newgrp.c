int	gbuf[259];
int	pbuf[259];
int	ttyb[3];
char	name[10];
char	passwd[10];
char	space[1000];
int	peek;
int	pwdflg;

main(argc, argv)
char *argv[];
{
	register id;
	register char *p, *q;

	if(argc &lt; 2) {
		printf(&quot;usage: newgrp groupname\n&quot;);
		done();
	}
	if(fopen(&quot;/etc/group&quot;, gbuf) &lt; 0) {
		printf(&quot;cannot open group file\n&quot;);
		done();
	}
	do {
		field(name, gbuf);
		p = name;
		q = argv[1];
		while(*p == *q++)
			if(*p++ == 0 || p &gt;= name+8)
				goto l1;
	} while(skip(gbuf));
	printf(&quot;%s: not a valid group name\n&quot;, argv[1]);
	done();

l1:
	if(fopen(&quot;/etc/passwd&quot;, pbuf) &lt; 0) {
		printf(&quot;cannot open password file\n&quot;);
		done();
	}
	p = space;
	id = getuid() &amp; 0377;
	do {
		field(p, pbuf);
		field(name, pbuf);
		if(value(pbuf) == id) {
			if(name[0] == 0)
				pwdflg++;
			while(*p++) ;
		}
	} while(skip(pbuf));
	*p = 0;

	field(passwd, gbuf);
	id = value(gbuf);
	if(id == 1) {
		pwdflg = 0;
		goto l2;
	}
	do {
		p = space;
		field(name, gbuf);
		while(*p) {
			q = name;
			while(*p == *q++) {
				if(*p++ == 0)
					goto l2;
			}
			while(*p++) ;
		}
	} while(peek == &#39;,&#39;);
	goto no;

l2:
	if(pwdflg &amp;&amp; passwd[0]) {
		printf(&quot;password: &quot;);
		gtty(0, pbuf);
		pbuf[3] = pbuf[2];
		pbuf[2] =&amp; ~010;
		stty(0, pbuf);
		read(0, gbuf, 512);
		pbuf[2] = pbuf[3];
		stty(0, pbuf);
		printf(&quot;\n&quot;);
		p = name;
		q = gbuf;
		while(p &lt; name+8) {
			if(*q == &#39;\n&#39;)
				break;
			*p++ = *q++;
		}
		*p = 0;
		p = crypt(name);
		q = passwd;
		while(q &lt; passwd+8)
			if(*p++ != *q++)
				goto no;
	}
	if(setgid(id) &lt; 0) {
		perror(&quot;setgid&quot;);
		goto no;
	}
	done();

no:
	printf(&quot;Sorry\n&quot;);
	done();
}

skip(buf)
{

	while(peek &gt; 0) {
		if(peek == &#39;\n&#39;) {
			peek = 1;
			return(1);
		}
		peek = getc(buf);
	}
	return(0);
}

field(cp, buf)
{
	register c;
	register char *p, *q;

	p = cp;
	q = p+8;
	while((c = getc(buf)) != &#39;\n&#39;) {
		if(c == &#39;,&#39; || c == &#39;:&#39; || c &lt;= 0)
			break;
		if(p &lt; q)
			*p++ = c;
	}
	*p = 0;
	peek = c;
}

value(buf)
{
	register n, c;

	n = 0;
	while((c = getc(buf)) &gt;= &#39;0&#39; &amp;&amp; c &lt;= &#39;9&#39;)
		n = n*10 + c-&#39;0&#39;;
	peek = c;
	return(n);
}

done()
{

	setuid(getuid());
	execl(&quot;/bin/sh&quot;, &quot;-&quot;, 0);
	exit();
}
