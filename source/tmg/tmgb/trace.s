i = r3
.globl putch,putoct
.globl trswitch
.globl trace

trace:
	jsr	pc,putch
	mov	i,r0
	jsr	pc,putoct
	mov	$&#39;\n,r0
	jsr	pc,putch
	rts	pc
.data
trswitch:	0
