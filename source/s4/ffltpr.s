/ C library-- fake floating output

.globl	pfloat
.globl	pscien

pfloat:
pscien:
	add	$8,r4
	movb	$&#39;?,(r3)+
	rts	pc
