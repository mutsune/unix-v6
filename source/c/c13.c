int opdope[] {
	000000,	/* EOF */
	000000,	/* ; */
	000000,	/* { */
	000000,	/* } */
	036000,	/* [ */
	002000,	/* ] */
	036000,	/* ( */
	002000,	/* ) */
	014201,	/* : */
	007001,	/* , */
	000000,	/* field selection */
	000000,	/* 11 */
	000000,	/* 12 */
	000000,	/* 13 */
	000000,	/* 14 */
	000000,	/* 15 */
	000000,	/* 16 */
	000000,	/* 17 */
	000000,	/* 18 */
	000000,	/* 19 */
	000400,	/* name */
	000400,	/* short constant */
	000400,	/* string */
	000400,	/* float */
	000400,	/* double */
	000000,	/* 25 */
	000000,	/* 26 */
	000400,	/* autoi, *r++ */
	000400,	/* autod, *--r */
	000000,	/* 29 */
	034203,	/* ++pre */
	034203,	/* --pre */
	034203,	/* ++post */
	034203,	/* --post */
	034220,	/* !un */
	034202,	/* &amp;un */
	034220,	/* *un */
	034200,	/* -un */
	034220,	/* ~un */
	036001,	/* . (structure reference) */
	030101,	/* + */
	030001,	/* - */
	032101,	/* * */
	032001,	/* / */
	032001,	/* % */
	026061,	/* &gt;&gt; */
	026061,	/* &lt;&lt; */
	020161,	/* &amp; */
	016161,	/* | */
	016161,	/* ^ */
	036001,	/* -&gt; */
	001000,	/* int -&gt; double */
	001000,	/* double -&gt; int */
	000001,	/* &amp;&amp; */
	000001,	/* || */
	030001, /* &amp;~ */
	001000,	/* double -&gt; long */
	001000,	/* long -&gt; double */
	001000,	/* integer -&gt; long */
	001000,	/* long -&gt; integer */
	022005,	/* == */
	022005,	/* != */
	024005,	/* &lt;= */
	024005,	/* &lt; */
	024005,	/* &gt;= */
	024005,	/* &gt; */
	024005,	/* &lt;p */
	024005,	/* &lt;=p */
	024005,	/* &gt;p */
	024005,	/* &gt;=p */
	012213,	/* =+ */
	012213,	/* =- */
	012213,	/* =* */
	012213,	/* =/ */
	012213,	/* =% */
	012253,	/* =&gt;&gt; */
	012253,	/* =&lt;&lt; */
	012253,	/* =&amp; */
	012253,	/* =| */
	012253,	/* =^ */
	012213,	/* = */
	030001, /* &amp; for tests */
	032101,	/*  * (long) */
	032001,	/*  / (long) */
	032001,	/* % (long) */
	012253,	/* =&amp; ~ */
	012213,	/* =* (long) */
	012213,	/* / (long) */
	012213,	/* % (long) */
	000000,	/* 89 */
	014201,	/* ? */
	026061,	/* long &lt;&lt; */
	012253,	/* long =&lt;&lt; */
	000000,	/* 93 */
	000000,	/* 94 */
	000000,	/* 95 */
	000000,	/* 96 */
	000000,	/* 97 */
	000000,	/* 98 */
	000000,	/* 99 */
	036001,	/* call */
	036000,	/* mcall */
	000000,	/* goto */
	000000,	/* jump cond */
	000000,	/* branch cond */
	000400,	/* set nregs */
	000000, /* 106 */
	000000,	/* 107 */
	000000,	/* 108 */
	000000,	/* 109 */
	000000	/* force r0 */
};

char	*opntab[] {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&quot;:&quot;,
	&quot;,&quot;,
	&quot;field select&quot;,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	&quot;name&quot;,
	&quot;short constant&quot;,
	&quot;string&quot;,
	&quot;float&quot;,
	&quot;double&quot;,
	0,
	0,
	&quot;*r++&quot;,
	&quot;*--r&quot;,
	0,
	&quot;++pre&quot;,
	&quot;--pre&quot;,
	&quot;++post&quot;,
	&quot;--post&quot;,
	&quot;!un&quot;,
	&quot;&amp;&quot;,
	&quot;*&quot;,
	&quot;-&quot;,
	&quot;~&quot;,
	&quot;.&quot;,
	&quot;+&quot;,
	&quot;-&quot;,
	&quot;*&quot;,
	&quot;/&quot;,
	&quot;%&quot;,
	&quot;&gt;&gt;&quot;,
	&quot;&lt;&lt;&quot;,
	&quot;&amp;&quot;,
	&quot;|&quot;,
	&quot;^&quot;,
	&quot;-&gt;&quot;,
	&quot;int-&gt;double&quot;,
	&quot;double-&gt;int&quot;,
	&quot;&amp;&amp;&quot;,
	&quot;||&quot;,
	&quot;&amp;~&quot;,
	&quot;double-&gt;long&quot;,
	&quot;long-&gt;double&quot;,
	&quot;integer-&gt;long&quot;,
	&quot;long-&gt;integer&quot;,
	&quot;==&quot;,
	&quot;!=&quot;,
	&quot;&lt;=&quot;,
	&quot;&lt;&quot;,
	&quot;&gt;=&quot;,
	&quot;&gt;&quot;,
	&quot;&lt;p&quot;,
	&quot;&lt;=p&quot;,
	&quot;&gt;p&quot;,
	&quot;&gt;=p&quot;,
	&quot;=+&quot;,
	&quot;=-&quot;,
	&quot;=*&quot;,
	&quot;=/&quot;,
	&quot;=%&quot;,
	&quot;=&gt;&gt;&quot;,
	&quot;=&lt;&lt;&quot;,
	&quot;=&amp;&quot;,
	&quot;=|&quot;,
	&quot;=^&quot;,
	&quot;=&quot;,
	&quot;&amp; for tests&quot;,
	&quot;*&quot;,
	&quot;/&quot;,
	&quot;%&quot;,
	&quot;=&amp; ~&quot;,
	&quot;=*&quot;,
	&quot;=/&quot;,
	&quot;=%&quot;,
	0,
	&quot;?&quot;,
	&quot;&lt;&lt;&quot;,
	&quot;=&lt;&lt;&quot;,
	0,
	0,
	0,
	0,
	0,
	&quot;call&quot;,
	&quot;call&quot;,
	&quot;call&quot;,
	0,
	&quot;goto&quot;,
	&quot;jump cond&quot;,
	&quot;branch cond&quot;,
	&quot;set nregs&quot;,
	&quot;load value&quot;,
	0,
	0,
	0,
	&quot;force register&quot;
};
