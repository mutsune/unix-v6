/
/

/ fxg -- declare implicit functions

.globl	funimpl

.globl	getname
.globl	lookup

funimpl:
	bit	$7,symtab(r3)
	bne	1f
	jsr	r5,getname
	mov	r2,-(sp)
	mov	$symbuf,r2
	jsr	r5,lookup; funtab
		br 2f
	cmpb	(r2),$&#39;\n
	bne	2f
	asr	r0
	movb	funtabt(r0),r0
	mov	r0,-(sp)
	bic	$!17,r0
	bis	typtab(r0),symtab(r3)
	mov	(sp)+,r0
	asr	r0
	asr	r0
	asr	r0
	asr	r0
	bic	$!17,r0
	mov	typtab(r0),symtab+2(r3)	/ save argument conversion
2:					/ in dimension pointer
	mov	(sp)+,r2
1:
	rts	r5

typtab:
	intcon
	realcon
	dblcon
	cplxcon
	dcplxcon

funtab:
	&lt;tanh\0&gt;
	&lt;sqrt\0&gt;
	&lt;sngl\0&gt;
	&lt;sin\0&gt;
	&lt;sign\0&gt;
	&lt;real\0&gt;
	&lt;mod\0&gt;
	&lt;min1\0&gt;
	&lt;min0\0&gt;
	&lt;max1\0&gt;
	&lt;max0\0&gt;
	&lt;isign\0&gt;
	&lt;int\0&gt;
	&lt;ifix\0&gt;
	&lt;idint\0&gt;
	&lt;idim\0&gt;
	&lt;iabs\0&gt;
	&lt;float\0&gt;
	&lt;exp\0&gt;
	&lt;dsqrt\0&gt;
	&lt;dsin\0&gt;
	&lt;dsign\0&gt;
	&lt;dreal\0&gt;
	&lt;dmod\0&gt;
	&lt;dmin1\0&gt;
	&lt;dmax1\0&gt;
	&lt;dlog10\0&gt;
	&lt;dlog\0&gt;
	&lt;dimag\0&gt;
	&lt;dim\0&gt;
	&lt;dexp\0&gt;
	&lt;dcsqrt\0&gt;
	&lt;dcsin\0&gt;
	&lt;dcos\0&gt;
	&lt;dconjg\0&gt;
	&lt;dcmplx\0&gt;
	&lt;dclog\0&gt;
	&lt;dcexp\0&gt;
	&lt;dccos\0&gt;
	&lt;dcabs\0&gt;
	&lt;dble\0&gt;
	&lt;datan2\0&gt;
	&lt;datan\0&gt;
	&lt;dabs\0&gt;
	&lt;csqrt\0&gt;
	&lt;csin\0&gt;
	&lt;cos\0&gt;
	&lt;conjg\0&gt;
	&lt;cmplx\0&gt;
	&lt;clog\0&gt;
	&lt;cexp\0&gt;
	&lt;ccos\0&gt;
	&lt;cabs\0&gt;
	&lt;atan2\0&gt;
	&lt;atan\0&gt;
	&lt;amod\0&gt;
	&lt;amin1\0&gt;
	&lt;amin0\0&gt;
	&lt;amax1\0&gt;
	&lt;amax0\0&gt;
	&lt;alog10\0&gt;
	&lt;alog\0&gt;
	&lt;aint\0&gt;
	&lt;aimag\0&gt;
	&lt;abs\0&gt;
	&lt;\0&gt;

/ function type xy
/	x = arg types
/	y = result type
/ 0 = integer
/ 2 = real
/ 4 = double
/ 6 = complex
/ 8 = doublecomplex
funtabt:
	.byte	2\&lt;4+2		/ tanh
	.byte	2\&lt;4+2		/ sqrt
	.byte	4\&lt;4+2		/ sngl
	.byte	2\&lt;4+2		/ sin
	.byte	2\&lt;4+2		/ sign
	.byte	6\&lt;4+2		/ real
	.byte	0\&lt;4+0		/ mod
	.byte	2\&lt;4+0		/ min1
	.byte	0\&lt;4+0		/ min0
	.byte	2\&lt;4+0		/ max1
	.byte	0\&lt;4+0		/ max0
	.byte	0\&lt;4+0		/ isign
	.byte	2\&lt;4+0		/ int
	.byte	2\&lt;4+0		/ ifix
	.byte	4\&lt;4+0		/ idint
	.byte	0\&lt;4+0		/ idim
	.byte	0\&lt;4+0		/ iabs
	.byte	0\&lt;4+2		/ float
	.byte	2\&lt;4+2		/ exp
	.byte	4\&lt;4+4		/ dsqrt
	.byte	4\&lt;4+4		/ dsin
	.byte	4\&lt;4+4		/ dsign
	.byte	8\&lt;4+4		/ dreal
	.byte	4\&lt;4+4		/ dmod
	.byte	4\&lt;4+4		/ dmin1
	.byte	4\&lt;4+4		/ dmax1
	.byte	4\&lt;4+4		/ dlog10
	.byte	4\&lt;4+4		/ dlog
	.byte	8\&lt;4+4		/ dimag
	.byte	2\&lt;4+2		/ dim
	.byte	4\&lt;4+4		/ dexp
	.byte	8\&lt;4+8		/ dcsqrt
	.byte	8\&lt;4+8		/ dcsin
	.byte	4\&lt;4+4		/ dcos
	.byte	8\&lt;4+8		/ dconjg
	.byte	4\&lt;4+8		/ dcmplx
	.byte	8\&lt;4+8		/ dclog
	.byte	8\&lt;4+8		/ dcexp
	.byte	8\&lt;4+8		/ dccos
	.byte	8\&lt;4+4		/ dcabs
	.byte	2\&lt;4+4		/ dble
	.byte	4\&lt;4+4		/ datan2
	.byte	4\&lt;4+4		/ datan
	.byte	4\&lt;4+4		/ dabs
	.byte	6\&lt;4+6		/ csqrt
	.byte	6\&lt;4+6		/ csin
	.byte	2\&lt;4+2		/ cos
	.byte	6\&lt;4+6		/ conjg
	.byte	2\&lt;4+6		/ cmplx
	.byte	6\&lt;4+6		/ clog
	.byte	6\&lt;4+6		/ cexp
	.byte	6\&lt;4+6		/ ccos
	.byte	6\&lt;4+2		/ cabs
	.byte	2\&lt;4+2		/ atan2
	.byte	2\&lt;4+2		/ atan
	.byte	2\&lt;4+2		/ amod
	.byte	2\&lt;4+2		/ amin1
	.byte	0\&lt;4+2		/ amin0
	.byte	2\&lt;4+2		/ amax1
	.byte	0\&lt;4+2		/ amax0
	.byte	2\&lt;4+2		/ alog10
	.byte	2\&lt;4+2		/ alog
	.byte	2\&lt;4+2		/ aint
	.byte	6\&lt;4+2		/ aimag
	.byte	2\&lt;4+2		/ abs
