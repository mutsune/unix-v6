begin:	ignore(blanks)
pr1:	comment\pr1
	parse(first)\pr2
	diag(error)
pr2:	comment\pr2
	parse(line)\pr2
	diag(error)\pr2
	putcharcl
	parse(last);

first:	parse(( fref = {&lt;1 &gt; 1 *}))
	getfref line = { 2&lt;:&gt; 1 };

error:	smark ignore(none) any(!&lt;&lt;&gt;&gt;) string(!&lt;&lt;;&gt;&gt;) scopy 
	( &lt;;&gt; = {&lt;;&gt;} | null )
	= { * &lt;??? &gt; 2 1 * };

line:
	labels
	( charcl &lt;;&gt;
	| statement
	| numbers
	| trule &lt;;&gt; )
	= { 2 * 1 * };

numbers: number &lt;;&gt; numbers/done = { 2 * 1 };

labels:	label labels/done = { 2 * 1 };

label:	name &lt;:&gt; = { 1 &lt;:&gt; };

last:	= {	&lt;.pn:1 .pxs;12&gt; *
		&lt;.tn:1 .txs;12&gt; * };

comment: &lt;/*&gt;
co1:	ignore(!&lt;&lt;*&gt;&gt;) &lt;*&gt; ignore(none) &lt;/&gt;/co1;

statement: [csym=0] oldtab(dtt) oldtab(pat)
	( proc plst tlst &lt;)&gt; = (1){2 1 }
	| = (1){} noelem )
stt1:	bundle	( frag = (1){ 2(nil) * 1(q1) }\stt1
		| &lt;;&gt;	( ifelem = { 1(xbit) }
			| ={ 1(nil) * &lt;1 succ&gt; }
		)	);

proc:	smark ignore(none) &lt;proc(&gt;;

plst:	list(pident)/null remote((octal(npa)))
	= { &lt;params;&gt; 1 * };

pident:	ident newtab(pat,npa);

tlst:	&lt;;&gt;/null [i=0] list((name [i++])) remote((octal(i)))
	= { &lt;push;&gt; 1 * 2 * };

frag:	prule = (1){ 1(nil,q1) }
	| labels noelem = (1){ 1 };

/*in sequel q2 is where to go on fail,q1 is exit bit*/

prule:	[sndt=ndt] disj
	( &lt;|&gt; [ndt=sndt] fref
		( ifeasy prule = (2){3(nil,nil)*&lt;salt;&gt;2*
				1(q2,q1)*2&lt;:&gt;}
		| prule fref = (2){4({*&lt;alt;&gt;1},q1)*&lt;goto;&gt;3*
				1&lt;:&gt;2(q2,q1)*3&lt;:&gt;} )
		noelem
	| () );

disj:	pelem pdot
	( disj = (2){2(q2,nil) * 1(nil,q1)} ifelem/done ishard
	| () );

pelem:	pprim = (2){1(q1)$2} iseasy
	| &lt;(&gt;	push(1,sndt)
		( prule &lt;)&gt;
		| &lt;)&gt; = (2){} noelem );

pprim:	( special
	| rname	( &lt;:&gt; fail
		| (spdot|()) ignore(none)
			( &lt;(&gt; ignore(blanks) list(parg) &lt;)&gt;
				= (1){$1 2 * 1}
			| = (1){$1 1}  )))
	( (&lt;/&gt; = {&lt;alt;&gt;} | &lt;\&gt;={&lt;salt;&gt;})
		rname = (1){3(nil)*$1 2 1}
	| () );

pdot:	&lt;.&gt;/done ignore(none) ident\alias
	([dtt?] | table(dtt) [ndt=0]) [ndt++];
spdot:	&lt;.&gt; ignore(none) not(( any(letter) ))
alias:	newtab(dtt,ndt);

parg:	rname | remote(specparg);

specparg: number
	| charcl
	| &lt;&lt;&gt; longlit
	| &lt;*&gt; = { &lt;\n\0&gt; }
	| &lt;(&gt; ( &lt;)&gt; = {&lt;1 succ&gt;}
		| push(3,dtt,ndt,sndt) [dtt=0]
			prule &lt;)&gt; oldtab(dtt)
			( ifelem = {1(nil,xbit) }
			| = {1(nil,nil)*&lt;1 succ&gt;} 
		)	);

iseasy:	[easy = 1];
ishard:	[easy = 0];
noelem:	[easy = 2];
ifelem:	[easy!=2?];
ifeasy:	[easy==1?];

special: &lt;=&gt; (rname | remote(trule))
		= (1){ $1 &lt;trans;1 &gt; 1 }
	| &lt;&lt;&gt; literal = (1){ $1 &lt;.px&gt; 1 }
	| &lt;*&gt; = (1){ $1 &lt;.pn&gt; }
	| &lt;[&gt; expr
		( &lt;?&gt; = {&lt;.t&gt;}
		| = {&lt;.p&gt;} )
		&lt;]&gt; = (1){ 2 * $1 1 };

rname:	( name tabval(pat,npa)/done
	| &lt;$&gt; number )
	= { &lt;[-&gt; 1 &lt;\&lt;1]&gt; };

trule:	oldtab(ptt)
	( tbody
	| &lt;(&gt; (number|tra) &lt;)&gt; tbody = {&lt;gpar;&gt; 2 * 1 } );
tra:	list(tident) octal(npt);

tident:	ident newtab(ptt,npt);

tbody: &lt;{&gt;	( &lt;}&gt; = { &lt;1 generate&gt; }
		| trb);
trb:	telem	( &lt;}&gt; = {  xbit 1 }
		| trb = { 2 * 1 } );

telem:	&lt;&lt;&gt; literal = { &lt;.tx&gt; 1 }
	| &lt;*&gt; = {&lt;.tn&gt;}
	| &lt;$&gt; number = { &lt;.tq;&gt; 1 }
	| number tdot = tpt
	| name te1\done te2\done;

te1:	tabval(dtt,ndt) tdot = tpt;
te2:	tabval(ptt,npt) = {&lt;.tq;&gt;1};

tdot:	(&lt;.&gt; number | ={&lt;0&gt;})
	( &lt;(&gt; list(targ) &lt;)&gt; | null)
	= { 2 &lt;;&gt; 1 };

targ:	name|remote(tbody);

tpt:	{ &lt;.tp;.byte &gt; 2 &lt;,&gt; 1 };

literal: ( shortlit
	| remote(longlit) = { &lt;;&gt; 1} );

shortlit: ignore(none) smark any(litch) &lt;&gt;&gt; scopy = { &lt;s;&#39;&gt; 1 };

longlit: ignore(none) (&lt;&gt;&gt; = { &lt;\&gt; &lt;&gt;&gt; } | null) litb &lt;&gt;&gt;
	= { &lt;&lt;&gt; 2 1 &lt;\0&gt; &lt;&gt;;.even&gt; };

litb:	smark string(litch) scopy &lt;\&gt;/done
	litb = { 2 &lt;\\&gt; 1 };

expr:	assignment | rv ;

assignment: lv assign expr = { 3 * 1 * 2 };

rv:	prime
rv1:	bundle	( infix prime = { 3 * 1 * 2 }\rv1
		| rva = { 2 * 1 }
		| () );
rva:	&lt;?&gt; rv &lt;:&gt; rv fref fref 
	= { &lt;.t;alt;&gt; 2 * 4 * &lt;salt;&gt; 1 * 2 &lt;:&gt; 3 * 1 &lt;:&gt; };

prime:
	lv suffix/done = { 2 * 1 }
	| prefix lv = { 1 * 2 }
	| &lt;(&gt; expr &lt;)&gt; 
	| unary prime = { 1 * 2 }
	| remote(number) = { &lt;.l;&gt; 1 };

lv:	( rname = { &lt;.l;&gt; 1 }
	| &lt;(&gt; lv &lt;)&gt;
	| &lt;*&gt; prime = { 1 * &lt;.rv&gt; } )
lv1:	&lt;[&gt;/done bundle expr &lt;]&gt; = { 2 * 1 * &lt;.f&gt; }\lv1;

assign:	&lt;=&gt; ignore(none) ( infix = { 1 * &lt;.u&gt; }
			| = { &lt;.st&gt; } );

infix:	smark ignore(none)
	( &lt;+&gt; not((&lt;+&gt; not((&lt;+&gt;)) )) = {&lt;.a&gt;}
	| &lt;-&gt; = {&lt;.s&gt;}
	| &lt;*&gt; = {&lt;.m&gt;}
	| &lt;/&gt; = {&lt;.q&gt;}
	| &lt;%&gt; = {&lt;.r&gt;}
	| &lt;|&gt; = {&lt;.o&gt;}
	| &lt;^&gt; = {&lt;.x&gt;}
	| &lt;&amp;&gt; = {&lt;.n&gt;}
	| &lt;==&gt; = {&lt;.eq&gt;}
	| &lt;!=&gt; = {&lt;.ne&gt;}
	| &lt;&lt;=&gt; = {&lt;.le&gt;}
	| &lt;&gt;=&gt; = {&lt;.ge&gt;}
	| &lt;&lt;&lt;&gt; = {&lt;.sl&gt;}
	| &lt;&lt;&gt; = {&lt;.lt&gt;}
	| &lt;&gt;&gt;	(  &lt;&gt;&gt; = {&lt;.sr&gt;}
		| = {&lt;.gt&gt;} )
	);

prefix:	smark ignore(none)
	( &lt;&amp;&gt; = {&lt;.lv&gt;}
	| &lt;++&gt; = {&lt;.ib&gt;}
	| &lt;--&gt; = {&lt;.db&gt;}
	);

suffix:	smark ignore(none)
	( &lt;++&gt; = {&lt;.ia&gt;}
	| &lt;--&gt; = {&lt;.da&gt;}
	);

unary:	( &lt;!&gt; = {&lt;.nt&gt;}
	| &lt;-&gt; = {&lt;.ng&gt;}
	| &lt;~&gt; = {&lt;.cm&gt;}
	);

charcl:
	( &lt;!&gt; ccla cclb | ccla )
	octal(classmask);
ccla:	(&lt;&lt;&lt;&gt;) [classmask = 1&lt;&lt;nclass++] [classmask?]/cherr
ccl1:	cclc &lt;&lt;&lt;&gt;\ccl1;
cclc:	ignore(none)

ccl3:	&lt;&gt;&gt;\ccl4 ccle\ccl3;
ccl4:	(&lt;&gt;&gt; | cclx fail) (not((&lt;&gt;&gt;)) | ccle);
ccle:	char(n) [*(2*n+&amp;classes) =| classmask];
cclb:	zeron
ccl5:	[*(&amp;classes+n) =^ classmask] testn\ccl5;
cclx:	[nclass--] zeron
ccl6:	[*(&amp;classes+n) =&amp; ~classmask] testn\ccl6;
cherr:	diag(( ={&lt;too many char classes&gt;} ));

zeron:	[n=0];
testn:	[(n=+2)&lt;400?];

putcharcl: zeron [classes=0] 
	parse(( = { * &lt;.globl classtab&gt; * &lt;classtab:&gt; * } ))
ptc1:	[w = *(&amp;classes+n)] parse((octal(w) = {1*}))
	bundle testn\ptc1;

classmask: 0;
nclass:	0;
classes:
cl1:	0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
cl2:	0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
cl3:	0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
cl4:	0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
cl5:	0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
cl6:	0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
cl7:	0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;
cl8:	0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;

done:	;

create:	[csym++]
getcsym: octal(csym) = {&lt;.&gt; 1};

fref:	[fsym++]
getfref: octal(fsym) = { &lt;..&gt; 1 };

not:	params(1) $1/done fail;

list:	params(1) $1
list1:	bundle &lt;,&gt;/done $1 = { 2 * 1 }\list1;

remote:	params(1) create parse(rem1,$1);
rem1:	params(1) getcsym $1 = { 2 &lt;=.&gt; * 1 * };

number: smark ignore(none) any(digit) string(digit) scopy;

name:	ident scopy;

ident:	smark ignore(none) any(letter) string(alpha);

oldtab:	params(1) [$1?]/done discard($1) [$1=0];

newtab:	params(2) ([$2?] | table($2) [$1=0])
	enter($2,i) [$2[i] = $1++];

tabval:	params(2) [$2?] find($2,i) [i=$1-$2[i]] octal(i);

null:	= nil;

xbit:	{&lt;1 &gt;};

q1:	{ $1 };
q2:	{ $2 };

nil:	{};

blanks:	&lt;&lt; 	
	&gt;&gt;;
digit:	&lt;&lt;0123456789&gt;&gt;;
letter:	&lt;&lt;abcdefghijklmnopqrstuvwxyz&gt;&gt;
	&lt;&lt;ABCDEFGHIJKLMNOPQRSTUVWXYZ&gt;&gt;;
alpha:	&lt;&lt;0123456789&gt;&gt;
	&lt;&lt;abcdefghijklmnopqrstuvwxyz&gt;&gt;
	&lt;&lt;ABCDEFGHIJKLMNOPQRSTUVWXYZ&gt;&gt;;
litch:	!&lt;&lt;\&gt;&gt;&gt;;
none:	&lt;&lt;&gt;&gt;;

csym:	0;
fsym:	0;
easy:	0;
w:	0;
n:	0;
dtt:	0;	/*delivered translation table*/
ndt:	0;	/*numb of delivered translations*/
sndt:	0;	/*saved ndt at beginning of disjunctive term*/
pat:	0;	/*parsing rule parameter table*/
npa:	0;	/*number of parsing rule params*/
ptt:	0;	/*table of params of translation*/
npt:	0;	/*number of params of translation*/
i:	0;
