/
/

/ fxf -- format statements

.globl	sform
.globl	sdata

.globl	ptemp
.globl	error

sform:
	cmp	progt,$6		/ block data
	bne	1f
	jsr	r5,error; 50.
1:
	jsr	r5,ptemp; &#39;f; efno; line
	rts	r5

sdata:
	jsr	r5,ptemp; &#39;d; efno; line
	rts	r5

