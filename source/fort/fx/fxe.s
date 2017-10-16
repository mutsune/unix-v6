/
/

/ xe -- temp file junk

.globl	tfildiag
.globl	tfil1, tfil2

tfildiag:
	mov	$1,r0
	sys	write; mes1; emes1-mes1
	clr	r0
	sys	seek; 0; 2
	mov	$-1,r0		/ failure return
	sys	exit

mes1:
	&lt;Temp file?\n&gt;
emes1:
tfil1:
	&lt;f.tmp1\0&gt;
tfil2:
	&lt;f.tmp2\0&gt;

