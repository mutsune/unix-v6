/ read and echo character from tty.
/ perform normal cr/lf uc/lc mapping.
tks = 177560
tkb = 177562
getc:
	tstb	*$tks
	bge	getc
	mov	tkb,r0
	bic	$!177,r0
	cmp	r0,$&#39;A
	blo	1f
	cmp	r0,$&#39;Z
	bhi	1f
	add	$&#39;a-&#39;A,r0
1:
	cmp	r0,$&#39;\r
	bne	putc
	mov	$&#39;\n,r0

/ put a character on the tty.
/ also performs delay.
tps = 177564
tpb = 177566
putc:
	cmp	r0,$&#39;\n
	bne	1f
	mov	$&#39;\r,r0
	jsr	pc,(r5)
	mov	$&#39;\n,r0
1:
	tstb	tps
	bpl	1b
	mov	r0,tpb
	rts	pc

/ write a string to tty
/ jsr pc, mesg; &lt;string\0&gt;; .even
mesg:
	movb	*(sp),r0
	beq	1f
	jsr	pc,(r5)
	inc	(sp)
	br	mesg
1:
	add	$2,(sp)
	bic	$1,(sp)
	rts	pc

callout:
	clr	r0
	cmp	(r0),$407
	bne	2f
1:
	mov	20(r0),(r0)+
	cmp	r0,sp
	blo	1b
2:
	mov	$start,-(sp)
	clr	pc

trvect:
	br	putc
	br	getc
	br	mesg
