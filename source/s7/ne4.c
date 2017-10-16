# include &quot;ne.h&quot;
# define	SIGPIPE	13	/* nroff has stopped reading */

int	ESC	033;
int	HREV	&#39;8&#39;;
int	HFWD	&#39;9&#39;;
int	SI	017;
int	SO	016;
int	ESCOUT	033;
int	HFWDOUT	&#39;9&#39;;
int	HREVOUT	&#39;8&#39;;
int	BKSPOUT	&#39;\b&#39;;
int	FWDOUT	&#39;~&#39;;

char	in[400];	/* input buffer */
int	exit();

main(argc,argv) int argc; char *argv[];{
	int i, type;
	flush();
	first = 0;
	lefteq = righteq = &#39;\0&#39;;
	signal(SIGPIPE, &amp;exit);
	setfile(argc,argv);
	while( (type=getline(in)) != &#39;\0&#39; ){
		eqline = linect;
		if( in[0]==&#39;.&#39; &amp;&amp; in[1]==&#39;E&#39; &amp;&amp; in[2]==&#39;Q&#39; ){
			for( i=11; i&lt;100; used[i++]=0 );
			printf(&quot;.tr ~\n&quot;);
			printf(&quot;%s&quot;,in);
			init();
			yyparse();
			if( eqnreg&gt;0 )
				printf(&quot;.ne %d\n.rs\n&#39;sp %d\n\\*(%d\n&#39;sp %d\n&quot;,
					(eqnht+1)/2, (eqnht-eqnbase-2)/2,eqnreg,eqnbase/2);
			printf(&quot;.EN&quot;);
			if( lastchar == &#39;\0&#39; ){
				putchar(&#39;\n&#39;);
				break;
			}
			if( putchar(lastchar) != &#39;\n&#39; )
				while( putchar(getc()) != &#39;\n&#39; );
			flush();
		} else if( type != lefteq )
			printf(&quot;%s&quot;,in);
		else
			inline();
	}
	putchar(&#39;\0&#39;);
	flush();
	exit();
}

getline(s) char *s; {
	char c;
	while((*s++=c=getc())!=&#39;\n&#39; &amp;&amp; c!=&#39;\0&#39; &amp;&amp; c!=lefteq );
	if( c==lefteq )
		s--;
	*s++ = &#39;\0&#39;;
	return(c);
}

inline() {
	int i,j,ds[20],t;
	i =  -1;
	do{
		if( i&gt;=17 ){
			while((j=getline(in))!=&#39;\n&#39; &amp;&amp; j!=&#39;\0&#39;);
			error(!FATAL,&quot;missing right delim (?) at %.20s&quot;,in);
			break;
		}
		ds[++i] = oalloc();
		printf(&quot;.ds %d \&quot;%s\n&quot;, ds[i], in);
		init();
		yyparse();
		if( eqnreg &gt; 0 )
			ds[++i] = eqnreg;
	} while( (t=getline(in)) == lefteq );
	ds[++i] = oalloc();
	printf(&quot;.ds %d \&quot;%s&quot;, ds[i], in);
	for( j=0; j&lt;=i; j++){
		printf(&quot;\\*(%d&quot;, ds[j]);
		ofree(ds[j]);
	}
	putchar(&#39;\n&#39;);
	flush();
}

putout(p1) int p1; {
	if(dbg)printf(&quot;.\tanswer &lt;- S%d\n&quot;,p1);
	eqnht = eht[p1];
	eqnbase = ebase[p1];
	eqnreg = p1;
}

abs(v) int v; {
	return( v&gt;0 ? v : -v );
}

max(i,j) int i,j; {
	return( i&gt;j ? i : j );
}

oalloc(){
	int i;
	for( i=11; i&lt;100; i++)
		if( used[i]++ == 0 ) return(i);
	error( FATAL, &quot;no strings left&quot;, i);
}

ofree(n) int n; {
	used[n] = 0;
}

setfile(argc, argv) int argc; char *argv[]; {
	svargc = --argc;
	svargv = argv;
	while( svargc &gt; 0 &amp;&amp; svargv[1][0] == &#39;-&#39;){
		switch( svargv[1][1] ){

		case &#39;d&#39;: lefteq=svargv[1][2]; righteq=svargv[1][3]; break;
		case &#39;s&#39;: break;
		case &#39;f&#39;: break;
		default:
			dbg = 1;
			ESCOUT = &#39;E&#39;;
			HFWDOUT = &#39;F&#39;;
			HREVOUT = &#39;R&#39;;
			BKSPOUT = &#39;B&#39;;
			FWDOUT = &#39;S&#39;;
		}
		svargc--;
		svargv++;
	}
	if( svargc == 0 )
		fin = dup(0);
	else if( (fin = open(svargv[1], 0)) &lt; 0)
		error( FATAL,&quot;can&#39;t open file %s&quot;, argv[1]);
	ptr = 0;
	fout = dup(1);
	ifile = 1;
	linect = 1;
}

yyerror(){;}

int	gsize	10;
int	gfont	&#39;I&#39;;

init(){
	ct = 0;
	first++;
}

error(fatal, s1, s2) int fatal; char *s1, *s2; {
	int sfout;
	printf(&quot;NEQN ERROR HERE&quot;);
	flush(fout);
	sfout = fout;
	fout = 2;
	if( fatal&gt;0 )
		printf(&quot;fatal error: &quot;);
	printf(s1,s2);
	printf(&quot; file %s, between lines %d and %d\n&quot;,
		 svargv[ifile], eqline, linect);
	flush(2);
	fout = sfout;
	if( fatal &gt; 0 )
		exit(1);
}

down(n) int n; {
	int c;
	if( n&lt;= 0 )
		c = HREVOUT;
	else
		c = HFWDOUT;
	n = abs(n);
	while( n-- &gt; 0 ){
		putchar(ESCOUT); putchar(c);
	}
}

up(n) int n; {
	int c;
	if( n&lt;= 0 )
		c = HFWDOUT;
	else
		c = HREVOUT;
	n = abs(n);
	while( n-- &gt; 0 ){
		putchar(ESCOUT); putchar(HREVOUT);
	}
}

fwd(n) int n; {
	int c,i;
	c = n&lt;0 ? BKSPOUT : FWDOUT;
	n = abs(n);
	while( n-- &gt; 0 )
		putchar(c);
}

back(n) int n; {
	int c,i;
	c = n&gt;0 ? BKSPOUT : FWDOUT;
	n = abs(n);
	while( n-- &gt; 0 )
		putchar(c);
}

line(n) int n; {
	while( n-- &gt; 0 )
		putchar(&#39;_&#39;);
}
