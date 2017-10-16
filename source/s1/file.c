int in;
int i 0;
char buf[512];
int *wd {
	&amp;buf[0]};
char *fort[]{
	&quot;function&quot;,&quot;subroutine&quot;,&quot;common&quot;,&quot;dimension&quot;,&quot;block&quot;,&quot;integer&quot;,
	&quot;real&quot;,&quot;data&quot;,&quot;double&quot;,0};
char *asc[]{
	&quot;sys&quot;,&quot;mov&quot;,&quot;tst&quot;,&quot;clr&quot;,&quot;jmp&quot;,0};
char *c[]{
	&quot;int&quot;,&quot;char&quot;,&quot;float&quot;,&quot;double&quot;,&quot;struct&quot;,&quot;extern&quot;,0};
char *as[]{
	&quot;globl&quot;,&quot;byte&quot;,&quot;even&quot;,&quot;text&quot;,&quot;data&quot;,&quot;bss&quot;,&quot;comm&quot;,0};
int ibuf[260];
main(argc, argv)
char **argv;
{

	while(argc &gt; 1) {
		printf(&quot;%s:	&quot;, argv[1]);
		type(argv[1]);
		argc--;
		argv++;
	}
}

type(file)
char *file;
{
	int j,nl;
	char ch;
	int mbuf[20];

	if(stat(file, mbuf) &lt; 0) {
		printf(&quot;cannot stat\n&quot;);
		return;
	}
	switch(mbuf[2]&amp;060000) {

	case 020000:
		printf(&quot;character&quot;);
		goto spcl;

	case 040000:
		printf(&quot;directory\n&quot;);
		return;

	case 060000:
		printf(&quot;block&quot;);

spcl:
		printf(&quot; special (%d/%d)\n&quot;,
		(mbuf[6]&gt;&gt;8)&amp;0377,
		mbuf[6]&amp;0377);
		return;
	}

	ibuf[0] = open(file, 0);
	if(ibuf[0] &lt; 0) {
		printf(&quot;cannot open\n&quot;);
		return;
	}
	in = read(ibuf[0], buf, 512);
	switch(*wd) {

	case 0407:
		printf(&quot;executable\n&quot;);
		goto out;

	case 0410:
		printf(&quot;pure executable\n&quot;);
		goto out;

	case 0411:
		printf(&quot;separate executable\n&quot;);
		goto out;

	case 0177555:
		printf(&quot;archive\n&quot;);
		goto out;
	}

	i = 0;
	if(ccom() == 0)goto notc;
	while(buf[i] == &#39;#&#39;){
		j = i;
		while(buf[i++] != &#39;\n&#39;){
			if(i - j &gt; 255){
				printf(&quot;data\n&quot;); 
				goto out;
			}
			if(i &gt;= in)goto notc;
		}
		if(ccom() == 0)goto notc;
	}
check:
	if(lookup(c) == 1){
		while((ch = buf[i++]) != &#39;;&#39; &amp;&amp; ch != &#39;{&#39;)if(i &gt;= in)goto notc;
		printf(&quot;c program&quot;);
		goto outa;
	}
	nl = 0;
	while(buf[i] != &#39;(&#39;){
		if(buf[i] &lt;= 0){
			printf(&quot;data\n&quot;); 
			goto out; 
		}
		if(buf[i] == &#39;;&#39;){
			i++; 
			goto check; 
		}
		if(buf[i++] == &#39;\n&#39;)
			if(nl++ &gt; 6)goto notc;
		if(i &gt;= in)goto notc;
	}
	while(buf[i] != &#39;)&#39;){
		if(buf[i++] == &#39;\n&#39;)
			if(nl++ &gt; 6)goto notc;
		if(i &gt;= in)goto notc;
	}
	while(buf[i] != &#39;{&#39;){
		if(buf[i++] == &#39;\n&#39;)
			if(nl++ &gt; 6)goto notc;
		if(i &gt;= in)goto notc;
	}
	printf(&quot;c program&quot;);
	goto outa;
notc:
	i = 0;
	while(buf[i] == &#39;c&#39; || buf[i] == &#39;#&#39;){
		while(buf[i++] != &#39;\n&#39;)if(i &gt;= in)goto notfort;
	}
	if(lookup(fort) == 1){
		printf(&quot;fortran&quot;);
		goto outa;
	}
notfort:
	i=0;
	if(ascom() == 0)goto notas;
	j = i-1;
	if(buf[i] == &#39;.&#39;){
		i++;
		if(lookup(as) == 1){
			printf(&quot;assembler program&quot;); 
			goto outa;
		}
		else if(buf[j] == &#39;\n&#39;){
			printf(&quot;roff, nroff, or eqn input&quot;);
			goto outa;
		}
	}
	while(lookup(asc) == 0){
		if(ascom() == 0)goto notas;
		while(buf[i] != &#39;\n&#39; &amp;&amp; buf[i++] != &#39;:&#39;)
			if(i &gt;= in)goto notas;
		while(buf[i] == &#39;\n&#39; || buf[i] == &#39; &#39; || buf[i] == &#39;\t&#39;)if(i++ &gt;= in)goto notas;
		j = i-1;
		if(buf[i] == &#39;.&#39;){
			i++;
			if(lookup(as) == 1){
				printf(&quot;assembler program&quot;); 
				goto outa; 
			}
			else if(buf[j] == &#39;\n&#39;){
				printf(&quot;roff, nroff, or eqn input&quot;);
				goto outa;
			}
		}
	}
	printf(&quot;assembler program&quot;);
	goto outa;
notas:
	for(i=0; i &lt; in; i++)if(buf[i] &lt;= 0){
		printf(&quot;data\n&quot;); 
		goto out; 
	}
	if((mbuf[2] &amp; 00111) != 0)
		printf(&quot;commands&quot;);
	else printf(&quot;probably text&quot;);
outa:
	while(i &lt; in)
		if(buf[i++] &lt;= 0){
			printf(&quot; with garbage\n&quot;);
			goto out;
		}
	while((in = read(ibuf[0],buf,512)) &gt; 0)
		for(i = 0; i &lt; in; i++)
			if(buf[i] &lt;= 0){
				printf(&quot; with garbage\n&quot;);
				goto out;
			}
	printf(&quot;\n&quot;);
out:
	close(ibuf[0]);
}
lookup(tab)
char *tab[];
{
	char r;
	int k,j,l;
	while(buf[i] == &#39; &#39; || buf[i] == &#39;\t&#39; || buf[i] == &#39;\n&#39;)i++;
	for(j=0; tab[j] != 0; j++){
		l=0;
		for(k=i; ((r=tab[j][l++]) == buf[k] &amp;&amp; r != &#39;\0&#39;);k++);
		if(r == &#39;\0&#39;)
			if(buf[k] == &#39; &#39; || buf[k] == &#39;\n&#39; || buf[k] == &#39;\t&#39;
			    || buf[k] == &#39;{&#39; || buf[k] == &#39;/&#39;){
				i=k;
				return(1);
			}
	}
	return(0);
}
ccom(){
	char cc;
	while((cc = buf[i]) == &#39; &#39; || cc == &#39;\t&#39; || cc == &#39;\n&#39;)if(i++ &gt;= in)return(0);
	if(buf[i] == &#39;/&#39; &amp;&amp; buf[i+1] == &#39;*&#39;){
		i =+ 2;
		while(buf[i] != &#39;*&#39; || buf[i+1] != &#39;/&#39;){
			if(buf[i] == &#39;\\&#39;)i =+ 2;
			else i++;
			if(i &gt;= in)return(0);
		}
		if((i =+ 2) &gt;= in)return(0);
	}
	if(buf[i] == &#39;\n&#39;)if(ccom() == 0)return(0);
	return(1);
}
ascom(){
	while(buf[i] == &#39;/&#39;){
		i++;
		while(buf[i++] != &#39;\n&#39;)if(i &gt;= in)return(0);
		while(buf[i] == &#39;\n&#39;)if(i++ &gt;= in)return(0);
	}
	return(1);
}
