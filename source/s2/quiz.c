#define NF 10
#define NL 200
#define NC 200
#define SL 100
#define NA 10

int tflag;
int xx[NL];
char score[NL];
int rights;
int wrongs;
int guesses;
int buf[259];
int nl 0;
int na NA;
int inc;
int ptr 0;
int nc 0;
char line[150];
char response[100];
char *tmp[NF];
int select[NF];

readline()
{
	char *t;
loop:
	for(t=line;(*t=getc(buf))!=-1;t++) {
		nc++;
		if(*t==&#39; &#39;&amp;&amp;(t==line||t[-1]==&#39; &#39;))
			t--;
		if(*t==&#39;\n&#39;) {
			if(t[-1]==&#39;\\&#39;)		/*inexact test*/
				continue;
			while(t&gt;line&amp;&amp;t[-1]==&#39; &#39;)
				*--t = &#39;\n&#39;;
			*++t = 0;
			return(1);
		}
		if(t-line&gt;=NC) {
			printf(&quot;Too hard for me\n&quot;);
			do {
				*line = getc(buf);
				if(*line==0377)
					return(0);
			} while(*line!=&#39;\n&#39;);
			goto loop;
		}
	}
	return(0);
}

char *eu;
char *ev;
cmp(u,v)
{
	int x;
	eu = u;
	ev = v;
	x = disj(1);
	if(x!=1)
		return(x);
	return(eat(1,0));
}

disj(s)
{
	int t, x;
	char *u;
	u = eu;
	t = 0;
	for(;;) {
		x = string(s);
		if(x&gt;1)
			return(x);
		switch(*ev) {
		case 0:
		case &#39;]&#39;:
		case &#39;}&#39;:
			return(t|x&amp;s);
		case &#39;|&#39;:
			ev++;
			t =| s;
			s = 0;
			continue;
		}
		if(s) eu = u;
		if(string(0)&gt;1)
			return(2);
		switch(*ev) {
		case 0:
		case &#39;]&#39;:
			return(0);
		case &#39;}&#39;:
			return(1);
		case &#39;|&#39;:
			ev++;
			continue;
		default:
			return(2);
		}
	}
}

string(s)
{
	int x;
	for(;;) {
		switch(*ev) {
		case 0:
		case &#39;|&#39;:
		case &#39;]&#39;:
		case &#39;}&#39;:
			return(1);
		case &#39;\\&#39;:
			ev++;
			if(*ev==0)
				return(2);
			if(*ev==&#39;\n&#39;) {
				ev++;
				continue;
			}
		default:
			if(eat(s,*ev)==1)
				continue;
			return(0);
		case &#39;[&#39;:
			ev++;
			x = disj(s);
			if(*ev!=&#39;]&#39; || x&gt;1)
				return(2);
			ev++;
			if(s==0)
				continue;
			if(x==0)
				return(0);
			continue;
		case &#39;{&#39;:
			ev++;
			x = disj(s);
			if(*ev!=&#39;}&#39;||x&gt;1)
				return(2);
			ev++;
			continue;
		}
	}
}

eat(s,c)
char c;
{
	if(*ev!=c)
		return(2);
	if(s==0) {
		ev++;
		return(1);
	}
	if(fold(*eu)!=fold(c))
		return(0);
	eu++;
	ev++;
	return(1);
}

fold(c)
char c;
{
	if(c&lt;&#39;A&#39;||c&gt;&#39;Z&#39;)
		return(c);
	return(c|040);
}

publish(t)
char *t;
{
	ev = t;
	pub1(1);
}

pub1(s)
{
	for(;;ev++){
		switch(*ev) {
		case &#39;|&#39;:
			s = 0;
			ev;
			continue;
		case &#39;]&#39;:
		case &#39;}&#39;:
		case 0:
			return;
		case &#39;[&#39;:
		case &#39;{&#39;:
			ev++;
			pub1(s);
			ev;
			continue;
		case &#39;\\&#39;:
			if(*++ev==&#39;\n&#39;)
				continue;
		default:
			if(s)
				putchar(*ev);
		}
	}
}

segment(u,w)
char *u, *w[];
{
	char *s;
	int i;
	char *t;
	s = u;
	for(i=0;i&lt;NF;i++) {
		u = s;
		t = w[i];
		while(*s!=&#39;:&#39;&amp;&amp;*s!=&#39;\n&#39;&amp;&amp;s-u&lt;SL) {
			if(*s==&#39;\\&#39;)  {
				if(s[1] == &#39;\n&#39;) {
					s =+ 2;
					continue;
				}
				*t++ = *s++;
			}
			*t++ = *s++;
		}

		while(*s!=&#39;:&#39;&amp;&amp;*s!=&#39;\n&#39;)
			s++;
		*t = 0;
		if(*s++==&#39;\n&#39;) {
			return(i+1);
		}
	}
	printf(&quot;Too many facts about one thing\n&quot;);
}

perm(u,m,v,n,p)
int p[];
char *u[], *v[];
{
	int i, j;
	int x;
	for(i=0;i&lt;m;i++) {
		for(j=0;j&lt;n;j++) {
			x = cmp(u[i],v[j]);
			if(x&gt;1) badinfo();
			if(x==0)
				continue;
			p[i] = j;
			goto uloop;
		}
		return(0);
uloop:		;
	}
	return(1);
}

find(u,m)
char *u[];
{
	int n;
	while(readline()){
		n = segment(line,tmp);
		if(perm(u,m,tmp+1,n-1,select))
			return(1);
	}
	return(0);
}

readindex()
{
	xx[0] = nc = 0;
	while(readline()) {
		xx[++nl] = nc;
		if(nl&gt;=NL) {
			printf(&quot;I&#39;ve forgotten some of it\n&quot;);
			break;
		}
	}
}

talloc()
{
	int i;
	for(i=0;i&lt;NF;i++)
		tmp[i] = alloc(SL);
}

main(argc,argv)
char *argv[];
{
	int i;
	int x;
	char *info;
	int tvec[2];
	char *t;
	extern done();
	int count;
	info = &quot;/usr/lib/quiz/index&quot;;
	time(tvec);
	inc = tvec[1]&amp;077774|01;
loop:
	if(argc&gt;1&amp;&amp;*argv[1]==&#39;-&#39;) {
		switch(argv[1][1]) {
		case &#39;i&#39;:
			if(argc&gt;2) 
				info = argv[2];
			argc =- 2;
			argv =+ 2;
			goto loop;
		case &#39;t&#39;:
			tflag = 1;
			argc--;
			argv++;
			goto loop;
		}
	}
	if(fopen(info,buf)== -1) {
		printf(&quot;No info\n&quot;);
		exit();
	}
	talloc();
	if(argc&lt;=2)
		instruct(info);
	signal(2,done);
	argv[argc] = 0;
	if(find(&amp;argv[1],argc-1)==0)
		dunno();
	close(buf[0]);
	if(fopen(tmp[0],buf)== -1) 
		dunno();
	readindex();
	if(!tflag || na&gt;nl)
		na = nl;
	for(;;) {
		i = next();
		seek(buf[0],xx[i],0);
		read(buf[0],line,xx[i+1]-xx[i]);
		segment(line,tmp);
		if(*tmp[select[0]] == &#39;\0&#39; || *tmp[select[1]] == &#39;\0&#39;) {
			score[i] = 1;
			continue;
		}
		publish(tmp[select[0]]);
		printf(&quot;\n&quot;);
		for(count=0;;count++) {
			if(query(response)==0) {
				publish(tmp[select[1]]);
				printf(&quot;\n&quot;);
				if(count==0) wrongs++;
				score[i] = tflag?-1:1;
				break;
			}
			x = cmp(response,tmp[select[1]]);
			if(x&gt;1) badinfo();
			if(x==1) {
				printf(&quot;Right!\n&quot;);
				if(count==0) rights++;
				if(++score[i]&gt;=1 &amp;&amp; na&lt;nl)
					na++;
				break;
			}
			printf(&quot;What?\n&quot;);
			if(count==0) wrongs++;
			score[i] = tflag?-1:1;
		}
		guesses =+ count;
	}
}

query(r)
char *r;
{
	char *t;
	for(t=r;;t++) {
		if(read(0,t,1)==0)
			done();
		if(*t==&#39; &#39;&amp;&amp;(t==r||t[-1]==&#39; &#39;))
			t--;
		if(*t==&#39;\n&#39;) {
			while(t&gt;r&amp;&amp;t[-1]==&#39; &#39;)
				*--t = &#39;\n&#39;;
			break;
		}
	}
	*t = 0;
	return(t-r);
}

next()
{
	int flag;
	inc = inc*3125&amp;077777;
	ptr = (inc&gt;&gt;2)%na;
	flag = 0;
	while(score[ptr]&gt;0)
		if(++ptr&gt;=na) {
			ptr = 0;
			if(flag) done();
			flag = 1;
		}
	return(ptr);
}

done()
{
	printf(&quot;\nRights %d, wrongs %d, &quot;, rights, wrongs);
	if(guesses)
		printf(&quot;extra guesses %d, &quot;, guesses);
	printf(&quot;score %d%%\n&quot;,100*rights/(rights+wrongs));
	exit();
}
instruct(info)
{
	char *t;
	int i, n;
	printf(&quot;Subjects:\n\n&quot;);
	while(readline()) {
		printf(&quot;-&quot;);
		n = segment(line,tmp);
		for(i=1;i&lt;n;i++) {
			printf(&quot; &quot;);
			publish(tmp[i]);
		}
		printf(&quot;\n&quot;);
	}
	printf(&quot;\n&quot;);
	if(fopen(info,buf)== -1)
		abort();
	readline();
	segment(line,tmp);
	printf(&quot;For example,\n&quot;);
	printf(&quot;    quiz &quot;);
	publish(tmp[1]);
	printf(&quot; &quot;);
	publish(tmp[2]);
	printf(&quot;\nasks you a &quot;);
	publish(tmp[1]);
	printf(&quot; and you answer the &quot;);
	publish(tmp[2]);
	printf(&quot;\n    quiz &quot;);
	publish(tmp[2]);
	printf(&quot; &quot;);
	publish(tmp[1]);
	printf(&quot;\nworks the other way around\n&quot;);
	printf(&quot;\nType empty line to get correct answer.\n&quot;);
	exit();
}

badinfo(){
	printf(&quot;Bad info %s\n&quot;,line);
}

dunno()
{
	printf(&quot;I don&#39;t know about that\n&quot;);
	exit();
}
