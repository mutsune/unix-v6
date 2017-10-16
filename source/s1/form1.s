signal = 48.
testing = 0
/	form letter generator
/	command sequence
/		form proto arg1 arg2...
/			where proto is letter
rfo:
	sys	signal; 2; int
	mov	$-1,buf
	mov	(sp)+,r2	/arg count
	tst	(sp)+		/skip first arg
	sub	$2,r2
	bge	2f
	mov	$default,-(sp)
2:
	mov	(sp)+,letter

1:	sys	stat; otfil; buf
	bec	2f
	sys	creat; otfil; 666
	bec	cont
2:	cmpb	$&#39;z,otfil+4
	beq	err3
	incb	otfil+4
	br	1b

err3:
	mov	$1,r0
	sys	write; mes3; 24.
	sys	exit
mes3:	&lt;cannot open output file\n&gt;
otfil:	&lt;forma\0&gt;
default:	&lt;letter\0&gt;
	.even

cont:	mov	r0,rfo
	clr	rfo+2
	jsr	pc,initl
	
/	set up argument list
/
	mov	$arg,r3
	mov	$10.,r0
	jsr	pc,allocate
	mov	r1,(r3)+
1:
	dec	r2
	blt	1f
	mov	$10.,r0
	jsr	pc,allocate
	mov	r1,(r3)+
	mov	(sp)+,r4
2:
	movb	(r4)+,r0
	beq	1b
	jsr	pc,putchar
	br	2b

1:
	sys	time
	ashc	$-7,r0
	div	$675.,r0
	mov	$daytab,r2
	sub	$1461.,r0

1:	sub	(r2)+,r0
	bge	1b
	add	-(r2),r0
	mov	r0,-(sp)
	mov	arg,r1
	mov	montab-daytab(r2),r2
1:
	movb	(r2)+,r0
	beq	1f
	jsr	pc,putchar
	br	1b
1:
	movb	$&#39; ,r0
	jsr	pc,putchar
	clr	r0
	mov	(sp),r1
	inc	r1
	div	$10.,r0
	mov	r1,(sp)
	mov	arg,r1
	tst	r0
	beq	1f
	add	$&#39;0,r0
	jsr	pc,putchar
1:
	mov	(sp)+,r0
	add	$&#39;0,r0
	jsr	pc,putchar
	jmp	loop

daytab: 31.; 28.; 31.; 30.; 31.; 30.; 31.; 31.; 30.; 31.; 30.; 999.

montab: m1; m2; m3; m4; m5; m6; m7; m8; m9; m10; m11; m12

m1:	&lt;January\0&gt;
m2:	&lt;February\0&gt;
m3:	&lt;March\0&gt;
m4:	&lt;April\0&gt;
m5:	&lt;May\0&gt;
m6:	&lt;June\0&gt;
m7:	&lt;July\0&gt;
m8:	&lt;August\0&gt;
m9:	&lt;September\0&gt;
m10:	&lt;October\0&gt;
m11:	&lt;November\0&gt;
m12:	&lt;December\0&gt;

buf:	.=.+34.
.=rfo+516.

loop:
	mov	$10.,r0
	jsr	pc,allocate
	mov	r1,opointer
	jsr	pc,allocate
	mov	r1,ipointer
	jsr	pc,allocate
	mov	r1,curly
	mov	$10.,r0
	jsr	pc,allocate
	mov	r1,scr
	mov	letter,r3
	movb	$&#39;\{,r0
	jsr	pc,putchar
1:
	movb	(r3)+,r0
	beq	1f
	jsr	pc,putchar
	br	1b
1:
	movb	$&#39;\},r0
	jsr	pc,putchar

	clr	switch

	tst	iflag
	beq	1f
	mov	$1,tflag
	jmp	interrupt
1:
	jsr	pc,scan

	movb	$&#39;\n,r0
	jsr	r5,putc
	jsr	r5,flushb

	mov	scr,r1
	jsr	pc,release
	jsr	pc,relarg
	mov	opointer,r1
	jsr	pc,release
	mov	ipointer,r1
	jsr	pc,release
	jsr	pc,relcurl
	jsr	pc,flush
	jsr	pc,whead
	.if	testing
	4
	.endif
	sys	exit
