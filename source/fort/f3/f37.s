/
/

/ f37 -- goto statements of all kinds

.globl	sgoto
.globl	intexp

.globl	e2
.globl	code
.globl	getsym
.globl	error
.globl	getlab
.globl	rvalue
.globl	convrt
.globl	chrtab

sgoto:
	movb	(r1),r0
	cmpb	chrtab(r0),$4		/ test digit
	bne	1f			/ not constant
	jsr	r5,getlab
		br 8f
	jsr	r5,code
		&lt;	goto; .%d\n\0&gt;; .even
		r0
	br	9f
1:
	cmpb	(r1),$&#39;(
	beq	1f
	jsr	r5,intexp
	cmp	r0,$36.			/ ,
	beq	2f
	jsr	r5,code
		&lt;	agoto\n\0&gt;; .even
	br	9f
2:
	jsr	r5,code
		&lt;	cagoto\n\0&gt;; .even
	cmpb	(r1)+,$&#39;(
	bne	8f			/ syntax
	jsr	pc,stmlist
	br	9f
1:
	mov	r1,r4
1:
	movb	(r1)+,r0
	beq	8f
	cmp	r0,$&#39;)
	bne	1b
	cmpb	(r1)+,$&#39;,
	bne	8f
	jsr	r5,intexp
	mov	r1,-(sp)
	jsr	r5,code
		&lt;	cgoto\n\0&gt;; .even
	mov	r4,r1
	tstb	(r1)+
	jsr	pc,stmlist
	mov	(sp)+,r1
9:
	tstb	(r1)
	beq	1f
8:
	jsr	r5,error; 35.
1:
	rts	r5

stmlist:
	jsr	r5,getlab
		br 9f
	jsr	r5,code
		&lt;	.%d\n\0&gt;; .even
		r0
	movb	(r1)+,r0
	cmp	r0,$&#39;,
	beq	stmlist
	cmp	r0,$&#39;)
	bne	8f
	jsr	r5,code
		&lt;	0\n\0&gt;; .even
	rts	pc
8:
	jsr	r5,error; 35.
9:
	rts	pc

intexp:
	jsr	r5,e2
	mov	r0,-(sp)
	jsr	r5,rvalue
	mov	$intcon,r2
	jsr	r5,convrt
	mov	(sp)+,r0
	rts	r5

