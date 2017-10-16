char number[3];
int eflg;
char w2006[100];
flg 0;
char realwd[100];
char *wd {&amp;realwd[1]};
char *fptr;
char *ffptr &amp;ffbuf;
char ffbuf[36];
int	neng;
int	npr;
int table[2];	/*keep these four cards in order*/
int tab1[26];
int tab2[730];
char tab3[19684];
int logtab[256];
float inctab[256];
char nwd[100];
int tot;
int wtot;
char *buf[3];
file[3];
ptr[3];
char *name[3];
bsp[768];

main(argc,argv) int argc; char *argv[]; {
	char let,lt;
	auto arg,t,sw,i,j,salt,er,c;
	register k,l,m;
	double junk;
	int unl();
	int ii;
	double log(), exp(), pow();

	nice(-20);
	inctab[0] = 1;
	logtab[0] = -10;
	for(ii=1; ii&lt;256; ii++){
		inctab[ii] = exp(-ii/30.497);
		logtab[ii] = log(30.*pow(1.0333,ii+0.) - 30.) + .5;
		}
	logtab[1] = -10;

	number[2] = &#39; &#39;;
	buf[0] = bsp;
	buf[1] = bsp + 0400;
	buf[2] = bsp + 01000;
	ptr[0] = 0; ptr[1] = 0;
	ptr[2] = 1;
	arg = 1;
	while(argc&gt;1 &amp;&amp; argv[arg][0] == &#39;-&#39;) {
		switch(argv[arg][1]) {
		default:
			printf(&quot;Unrecognizable argument: %c\n&quot;,argv[arg][1]);
				exit();
		case 0:
		case &#39;n&#39;:
				neng++;
				break;
		case &#39;1&#39;:
				npr++;
		}
		arg++;
		--argc;
	}
	if(!neng) {
		salt = open(&quot;/usr/lib/salt&quot;,0);
		er = read(salt,table,21200);
			if(er != 21200)err(&quot;read salt&quot;);
		close(salt);
		}
	if((signal(2,1) &amp; 1) != 1)
	signal(2,unl);
	name[0] = &quot;/usr/tmp/ttmpa1&quot;;
	name[1] = &quot;/usr/tmp/ttmpa2&quot;;
	name[2] = &quot;/usr/tmp/ttmpa3&quot;;
	while((file[0] = open(name[0],1)) &gt; 0){
		close(file[0]);
		for(j=0; j &lt; 3; j++)name[j][13]++;
		if(name[0][13] == &#39;z&#39;)err(&quot;creat tmp file&quot;);
	}
	file[0] = creat(name[0],0666);
	fptr = argv[arg];
	if(argc == 1) {argc = 2; arg = 0;}
	while(--argc){
		if(arg == 0){
			file[2] = 0;
		}else{
			file[2] = open(argv[arg++],0);
			if(file[2] &lt; 0)err(&quot;open input file&quot;);
		}
		eflg = 1;
		while((j = wdval(2)) != 0){
			put(0,nwd,j);
			k = -1;
			l = 0;
			m = 1;
			if(inctab[table[0]] &gt; (junk=rand()/32768.)) table[0]++;
			while(m &lt;= j){
				c = 27*wd[k++] + wd[l++];
				if(inctab[tab2[c]] &gt; junk) tab2[c]++;
				c = 27*c + wd[m++];
				if(inctab[tab3[c]] &gt; junk) tab3[c]++;
			}
			c = 27*wd[k] + wd[l];
			if(inctab[tab2[c]] &gt; junk) tab2[c]++;
		}
		if(file[2]) close(file[2]);
	}
	flsh(0,0);
	close(file[0]);
	sw = fork();
		if(sw == 0){execl(&quot;/usr/bin/usort&quot;,&quot;usort&quot;,&quot;-o&quot;,name[2],name[0],0);
			err(&quot;sort&quot;); }
		if(sw == -1)err(&quot;fork&quot;);
	er = wait();
		if(er != sw)err(&quot;probs&quot;);
	file[0] = creat(name[0],0666);
		if(file[0] &lt; 0)err(&quot;creat tmp&quot;);
	file[1] = open(&quot;/usr/lib/w2006&quot;,0);
		if(file[1] &lt; 0)err(&quot;open w2006&quot;);
	ptr[1] = 1;
	for(k=0;((c = w2006[k] = get(1)) != &#39;\n&#39;);k++)
		if(c == -1) break;
	file[2] = open(name[2],0);
	if(file[2] &lt; 0)err(&quot;open tmp&quot;);
	ptr[2] = 1;

	while(ptr[2]){
		l=0;
		for(k=0;((c = wd[k] = get(2)) != &#39;\n&#39;);k++)
			if(c == -1)goto done;
		for(i=0; i&lt;=k;i++){
			if(wd[i] &lt; w2006[l]){
				put(0,wd,k);
				break;
			}
			if(wd[i] &gt; w2006[l]){
				for(l=0;((c = w2006[l] = get(1)) != &#39;\n&#39;);l++)
				if(c == -1){
					put(0,wd,k);
					for(k=0;((c = wd[k] =get(2))!= -1);k++){
						put(0,wd,k);
						k = -1;
					}
					goto done;
				}
				i = -1;
				l=0;
				continue;
			}
			l++;
		}
	}
done:
	close(file[2]); 
	unlink(name[2]);
	flsh(0,0);
	close(file[1]);
	close(file[0]);
	ptr[1] = 1;
	file[1] = open(name[0],0);
		if(file[1] &lt; 0)err(&quot;open tmp &quot;);
	file[0] = creat(name[1],0666);
		if(file[0] &lt; 0)err(&quot;create tmp&quot;);
	while((j = nwdval(1)) != 0){
		wtot = 0;
		flg = 0;
		k = -1; l = 0; m = 1;
		while(m &lt;= j){
			tot = 0;
			c = wd[k++]*27 + wd[l++];
			tot =+ (logtab[tab2[c]]+logtab[tab2[wd[k]*27+wd[l]]]);
			tot =&gt;&gt; 1;
			c = c*27 + wd[m++];
			tot =- logtab[tab3[c] &amp; 0377];
			if(tot &gt; wtot) wtot = tot;
			}
		if(wtot &lt; 0) wtot = 0;
		t = conf(wtot,2,number);
		put(0,number,2);
		put(0,nwd,j);
		}
	flsh(0,0);
	close(file[1]);
	close(file[0]);

	sw = fork();
		if(sw == 0){execl(&quot;/bin/sort&quot;,&quot;sort&quot;,&quot;+0nr&quot;, &quot;+1&quot;,&quot;-o&quot;,name[1],name[1]
			,0);
			err(&quot;sort&quot;); }
		if(sw == -1)err(&quot;fork&quot;);
	er = wait();
		if(er != sw)err(&quot;prob&quot;);

	sw = fork();
		if(sw == 0){
			if(npr) {
				execl(&quot;/bin/cat&quot;,&quot;cat&quot;,name[1],0);
			} else {
				i = 0 ;
				while((c = &quot;Possible typo&#39;s in &quot;[i++])!=0)
					*ffptr++ = c;
				i = 0;
				while((c = fptr[i++]) != 0)
					*ffptr++ = c;
				*ffptr = 0;
				execl(&quot;/bin/pr&quot;,&quot;pr&quot;,&quot;-3&quot;, &quot;-h&quot;,
				ffbuf,name[1],0);
				err(&quot;pr&quot;);
		}
	}
		if(sw == -1)err(&quot;fork&quot;);
	er = wait();
		if(er != sw)err(&quot;prob&quot;);
	unl();
}

unl() {
	register j;
	j = 2;
	while(j--)unlink(name[j]);
	exit();
}


err(c) char c[];{
	register j;
	printf(&quot;cannot %s\n&quot;,c);
	unl();
}

get(ifile) int ifile;{
	static char *ibuf[10];
	if(--ptr[ifile]){
		return(*ibuf[ifile]++ &amp; 0377);}
	if(ptr[ifile] = read(file[ifile],buf[ifile],512)){
		if(ptr[ifile] &lt; 0)goto prob;
		ibuf[ifile] = buf[ifile];
		return(*ibuf[ifile]++ &amp; 0377);
	}
	ptr[ifile] = 1;
	return(-1);

prob:
	ptr[ifile] = 1;
	printf(&quot;read error\n&quot;);
	return(-1);
}

put(ofile,s,optr) char s[]; {
	register i;

	while(optr-- &gt;= 0)
		 buf[ofile][(ptr[ofile] &lt; 512)?ptr[ofile]++:flsh(ofile,1)] = *s++;
	return;
}

flsh(ofile,i){
	register error;
	error = write(file[ofile],buf[ofile],ptr[ofile]);
	if(error &lt; 0)goto prob;

	ptr[ofile] = i;
	return(0);
prob:
	printf(&quot;write error on t.%d\n&quot;,file[ofile]);
	unl();
}

wdval(wfile) int wfile; {
	static let,wflg;
	register j;
beg:
	j = -1;
	if(wflg == 1){wflg = 0;
		goto st; }
	while((let = get(wfile)) != &#39;\n&#39;){
st:
		switch(let){
		case -1:	return(0);
		case &#39;%&#39;:	if(j != -1)break;
					goto ret;
		case &#39;-&#39;:
				if((let = get(wfile)) == &#39;\n&#39;){
					while((let = get(wfile)) == &#39;\n&#39;)if(let == -1)return(0);
					goto st; }
				else {wflg = 1;
					goto ret; }
		case &#39;\&#39;&#39;:
				if(eflg != 1){
					if(j &lt; 1)goto beg;
						else break;
					}
		case &#39;.&#39;:
				if(eflg == 1){
					while((let = get(wfile)) != &#39;\n&#39;)if(let == -1)return(0);
					goto beg; }
				else goto ret;
		default:
				eflg = 0;
				if(let &lt; &#39;A&#39;)goto ret;
				if(let &lt;= &#39;Z&#39;){ wd[++j] = let - 0100;
					nwd[j] = let + &#39; &#39;;
					break; }
				if(let &lt; &#39;a&#39; || let &gt; &#39;z&#39;)goto ret;
				wd[++j] = let - 0140;
				nwd[j] = let;
			}
		 eflg = 0;	}

	eflg = 1;
ret:
	if(j &lt; 1)goto beg;
	nwd[++j] = &#39;\n&#39;;
	wd[j] = 0;
	return(j);
}

nwdval(wfile) int wfile;{
	register j;
	register char c;
	j = -1;
	do{
		if(( c = nwd[++j] = get(wfile)) == -1)return(0);
		wd[j] = c - 0140;
	}
	while(c != &#39;\n&#39;);
	wd[j] = &#39;\0&#39;;
	return(j);
}
conf(n,width,cbuf) char cbuf[]; {
	register i,a;

	i = width;
	while(i--)cbuf[i] = &#39; &#39;;

	cbuf[(a = n/10)?conf(a,--width,cbuf):--width] = n%10 + &#39;0&#39;;

	return(++width);
}
rand(){
	static gorp;
	gorp = (gorp + 625) &amp; 077777;
	return(gorp);
}
