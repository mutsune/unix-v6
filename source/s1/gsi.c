#define	PLOT	006	/* ack */
#define BEL    007     /* exit plot mode */
#define	ESC	033	/* escape */
#define	HFWD	&#39;9&#39;
#define	HREV	&#39;8&#39;
#define	FREV	&#39;7&#39;
#define	SO	016	/* shift out - enter greek */
#define	SI	017	/* shift in */
#define	UP	013
#define	DN	012
#define	RT	&#39; &#39;
#define	LF	&#39;\b&#39;

int	restore();
int	svmode, mode[3];

main(argc,argv) int argc; char **argv; {
	int c, textmode;
	extern int fin,fout;
	fin = dup(0);
	fout = dup(1);
	if ((signal(2, 1) &amp; 01) == 0)
		signal(2, &amp;restore);
	gtty(1, mode);
	svmode = mode[2];
	mode[2] =&amp; 0777757;	/* turn off cr-lf  */
	mode[2] =| 03;	/* turn off delays, tabs */
	stty(1, mode);
	textmode = 1;
	while( (c=getchar()) != &#39;\0&#39; ){
		if( c==SO ){
			special();
			continue;
		}
		if (c== PLOT) textmode = 1-textmode;
		if (c==BEL) textmode = 1;
		if( c==&#39;\n&#39; &amp;&amp; textmode )
			putchar(015);	/* CR */
		if( c!=ESC ){
			putchar(c);
			continue;
		}
		putchar(PLOT);
		c = getchar();
		if( c == HREV )
			nplot(4,UP);
		else if( c == HFWD )
			nplot(4,DN);
		else if( c == FREV )
			nplot(8,UP);
		putchar(PLOT);
	}
	flush();
	restore();
}

restore(){
	mode[2] = svmode;
	stty(1, mode);
	exit();
}

int	tab[]{
	&#39;A&#39;,	/* alpha */
	&#39;B&#39;,	/* beta */
	&#39;D&#39;,	/* delta */
	&#39;W&#39;,	/* DELTA */
	&#39;S&#39;,	/* epsilon */
	&#39;N&#39;,	/* eta */
	&#39;\\&#39;,	/* gamma */
	&#39;G&#39;,	/* GAMMA */
	&#39;o&#39;,	/* infinity - not in M37 */
	&#39;^&#39;,	/* integral */
	&#39;L&#39;,	/* lambda */
	&#39;E&#39;,	/* LAMBDA */
	&#39;M&#39;,	/* mu */
	&#39;[&#39;,	/* nabla (del) */
	&#39;_&#39;,	/* not */
	&#39;@&#39;,	/* nu */
	&#39;C&#39;,	/* omega */
	&#39;Z&#39;,	/* OMEGA */
	&#39;]&#39;,	/* partial */
	&#39;U&#39;,	/* phi */
	&#39;F&#39;,	/* PHI */
	&#39;V&#39;,	/* psi */
	&#39;H&#39;,	/* PSI */
	&#39;J&#39;,	/* pi */
	&#39;P&#39;,	/* PI */
	&#39;K&#39;,	/* rho */
	&#39;Y&#39;,	/* sigma */
	&#39;R&#39;,	/* SIGMA */
	&#39;I&#39;,	/* tau */
	&#39;T&#39;,	/* theta */
	&#39;O&#39;,	/* THETA */
	&#39;X&#39;,	/* xi */
	&#39;Q&#39;,	/* zeta */
	0
};
int	trans[]{
	alpha,
	beta,
	delta,
	DELTA,
	epsilon,
	eta,
	gamma,
	GAMMA,
	infinity,
	integral,
	lambda,
	LAMBDA,
	mu,
	nabla,
	not,
	nu,
	omega,
	OMEGA,
	partial,
	phi,
	PHI,
	psi,
	PSI,
	pi,
	PI,
	rho,
	sigma,
	SIGMA,
	tau,
	theta,
	THETA,
	xi,
	zeta,
	0
};

int alpha[]	{LF,&#39;c&#39;,RT,RT,&#39;(&#39;,LF,0};
int beta[]	{&#39;B&#39;,LF,LF,DN,DN,&#39;|&#39;,RT,RT,UP,UP,0};
int delta[]	{&#39;o&#39;,UP,UP,&#39;&lt;&#39;,DN,DN,0};
int DELTA[]	{LF,LF,&#39;/&#39;,-3,DN,&#39;-&#39;,-4,RT,&#39;-&#39;,-3,UP,&#39;\\&#39;,LF,LF,0};
int epsilon[]	{&#39;&lt;&#39;,&#39;-&#39;,0};
int eta[]	{&#39;n&#39;,RT,RT,DN,DN,&#39;|&#39;,LF,LF,UP,UP,0};
int gamma[]	{&#39;)&#39;,RT,&#39;/&#39;,LF,0};
int GAMMA[]	{LF,LF,&#39;|&#39;,RT,RT,-3,UP,&#39;-&#39;,-3,DN,RT,RT,&#39;`&#39;,LF,LF,0};
int infinity[]	{LF,LF,&#39;c&#39;,-4,RT,&#39;o&#39;,LF,LF,0};
int integral[]	{&#39;|&#39;,&#39;\&#39;&#39;,RT,RT,&#39;`&#39;,-3,LF,-6,DN,&#39;\&#39;&#39;,LF,&#39;`&#39;,RT,RT,-6,UP,0};
int lambda[]	{&#39;\\&#39;,-4,DN,LF,&#39;\&#39;&#39;,DN,LF,&#39;\&#39;&#39;,-5,UP,RT,RT,0};
int LAMBDA[]	{LF,LF,&#39;/&#39;,-4,RT,&#39;\\&#39;,LF,LF,0};
int mu[]	{&#39;u&#39;,LF,LF,&#39;,&#39;,RT,RT,0};
int nabla[]	{LF,LF,&#39;\\&#39;,-3,UP,&#39;-&#39;,-4,RT,&#39;-&#39;,-3,DN,&#39;/&#39;,LF,LF,0};
int not[]	{&#39;-&#39;,-2,RT,UP,&#39;,&#39;,DN,-2,LF,0};
int nu[]	{LF,&#39;(&#39;,-3,RT,&#39;/&#39;,LF,LF,0};
int omega[]	{LF,&#39;u&#39;,-3,RT,&#39;u&#39;,LF,LF,0};
int OMEGA[]	{&#39;O&#39;,DN,DN,LF,&#39;-&#39;,RT,RT,&#39;-&#39;,LF,UP,UP,0};
int partial[]	{&#39;o&#39;,RT,DN,&#39;`&#39;,LF,UP,&#39;`&#39;,LF,UP,&#39;`&#39;,RT,DN,0};
int phi[]	{&#39;o&#39;,&#39;/&#39;,0};
int PHI[]	{&#39;o&#39;,&#39;[&#39;,&#39;]&#39;,0};
int psi[]	{&#39;/&#39;,&#39;-&#39;,DN,DN,RT,RT,&#39;\&#39;&#39;,-4,LF,&#39;\&#39;&#39;,RT,RT,UP,UP,0};
int PSI[]	{&#39;[&#39;,&#39;]&#39;,&#39;-&#39;,DN,DN,RT,RT,&#39;\&#39;&#39;,-4,LF,&#39;`&#39;,RT,RT,UP,UP,0};
int pi[]	{UP,&#39;-&#39;,-3,DN,&#39;&quot;&#39;,DN,&#39;&quot;&#39;,-3,UP,0};
int PI[]	{LF,LF,&#39;[&#39;,&#39;]&#39;,-4,RT,&#39;[&#39;,&#39;]&#39;,LF,LF,-3,UP,&#39;-&#39;,-3,DN,0};
int rho[]	{&#39;o&#39;,LF,LF,DN,DN,&#39;|&#39;,UP,UP,RT,RT,0};
int sigma[]	{&#39;o&#39;,DN,RT,RT,&#39;~&#39;,UP,LF,LF,0};
int SIGMA[]	{&#39;&gt;&#39;,-2,DN,&#39;-&#39;,-5,UP,&#39;-&#39;,-3,DN,0};
int tau[]	{&#39;t&#39;,DN,RT,RT,&#39;~&#39;,LF,LF,LF,&#39;~&#39;,RT,UP,0};
int theta[]	{&#39;O&#39;,&#39;-&#39;,0};
int THETA[]	{&#39;O&#39;,&#39;=&#39;,0};
int xi[]	{&#39;c&#39;,RT,DN,&#39;,&#39;,LF,-3,UP,&#39;c&#39;,LF,DN,&#39;`&#39;,RT,DN,0};
int zeta[]	{&#39;c&#39;,RT,DN,&#39;,&#39;,LF,-3,UP,&#39;&lt;&#39;,DN,DN,0};

special(){
	int c,i,j,t;
   loop:
	if( (c=getchar()) == SI )
		return;
	for( i=0; tab[i]!=0; i++)
		if( c==tab[i] ){
			plot(trans[i]);
			goto loop;
		}
	putchar(c);
	goto loop;
}

plot(s) int *s; {
	int i,c;
	putchar(PLOT);
	for( i=0; (c=s[i])!=0; i++ )
		if( c&lt;0 )
			nplot(-c,s[++i]);
		else
			putchar(c);
	putchar(PLOT);
	putchar(&#39; &#39;);
}

nplot(n,c) int n,c; {
	while(n--)
		putchar(c);
}
