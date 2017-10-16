/
/

/ a9 -- pdp-11 assembler pass 1

eae = 0

/ key to types

/	0	undefined
/	1	absolute
/	2	text
/	3	data
/	4	bss
/	5	flop freg,dst (movfo, = stcfd)
/	6	branch
/	7	jsr
/	10	rts
/	11	sys
/	12	movf (=ldf,stf)
/	13	double operand (mov)
/	14	flop fsrc,freg (addf)
/	15	single operand (clr)
/	16	.byte
/	17	string (.ascii, &quot;&lt;&quot;)
/	20	.even
/	21	.if
/	22	.endif
/	23	.globl
/	24	register
/	25	.text
/	26	.data
/	27	.bss
/	30	mul,div, etc
/	31	sob
/	32	.comm
/	33	estimated text
/	34	estimated data
/	35	jbr
/	36	jeq, jne, etc

	.data
symtab:
/ special variables

&lt;.\0\0\0\0\0\0\0&gt;; dotrel:02; dot:000000
&lt;..\0\0\0\0\0\0&gt;;	01; dotdot:000000

/ register

&lt;r0\0\0\0\0\0\0&gt;;	24;000000
&lt;r1\0\0\0\0\0\0&gt;;	24;000001
&lt;r2\0\0\0\0\0\0&gt;;	24;000002
&lt;r3\0\0\0\0\0\0&gt;;	24;000003
&lt;r4\0\0\0\0\0\0&gt;;	24;000004
&lt;r5\0\0\0\0\0\0&gt;;	24;000005
&lt;sp\0\0\0\0\0\0&gt;;	24;000006
&lt;pc\0\0\0\0\0\0&gt;;	24;000007

.if eae

/eae &amp; switches

&lt;csw\0\0\0\0\0&gt;;	01;177570
&lt;div\0\0\0\0\0&gt;;	01;177300
&lt;ac\0\0\0\0\0\0&gt;;	01;177302
&lt;mq\0\0\0\0\0\0&gt;;	01;177304
&lt;mul\0\0\0\0\0&gt;;	01;177306
&lt;sc\0\0\0\0\0\0&gt;;	01;177310
&lt;sr\0\0\0\0\0\0&gt;;	01;177311
&lt;nor\0\0\0\0\0&gt;;	01;177312
&lt;lsh\0\0\0\0\0&gt;;	01;177314
&lt;ash\0\0\0\0\0&gt;;	01;177316

.endif

/ system calls

&lt;exit\0\0\0\0&gt;;		01;0000001
&lt;fork\0\0\0\0&gt;;		01;0000002
&lt;read\0\0\0\0&gt;;		01;0000003
&lt;write\0\0\0&gt;;		01;0000004
&lt;open\0\0\0\0&gt;;		01;0000005
&lt;close\0\0\0&gt;;		01;0000006
&lt;wait\0\0\0\0&gt;;		01;0000007
&lt;creat\0\0\0&gt;;		01;0000010
&lt;link\0\0\0\0&gt;;		01;0000011
&lt;unlink\0\0&gt;;		01;0000012
&lt;exec\0\0\0\0&gt;;		01;0000013
&lt;chdir\0\0\0&gt;;		01;0000014
&lt;time\0\0\0\0&gt;;		01;0000015
&lt;makdir\0\0&gt;;		01;0000016
&lt;chmod\0\0\0&gt;;		01;0000017
&lt;chown\0\0\0&gt;;		01;0000020
&lt;break\0\0\0&gt;;		01;0000021
&lt;stat\0\0\0\0&gt;;		01;0000022
&lt;seek\0\0\0\0&gt;;		01;0000023
&lt;tell\0\0\0\0&gt;;		01;0000024
&lt;mount\0\0\0&gt;;		01;0000025
&lt;umount\0\0&gt;;		01;0000026
&lt;setuid\0\0&gt;;		01;0000027
&lt;getuid\0\0&gt;;		01;0000030
&lt;stime\0\0\0&gt;;		01;0000031
&lt;fstat\0\0\0&gt;;		01;0000034
&lt;mdate\0\0\0&gt;;		01;0000036
&lt;stty\0\0\0\0&gt;;		01;0000037
&lt;gtty\0\0\0\0&gt;;		01;0000040
&lt;nice\0\0\0\0&gt;;		01;0000042
&lt;signal\0\0&gt;;		01;0000060

/ double operand

&lt;mov\0\0\0\0\0&gt;;	13;0010000
&lt;movb\0\0\0\0&gt;;		13;0110000
&lt;cmp\0\0\0\0\0&gt;;	13;0020000
&lt;cmpb\0\0\0\0&gt;;		13;0120000
&lt;bit\0\0\0\0\0&gt;;	13;0030000
&lt;bitb\0\0\0\0&gt;;		13;0130000
&lt;bic\0\0\0\0\0&gt;;	13;0040000
&lt;bicb\0\0\0\0&gt;;		13;0140000
&lt;bis\0\0\0\0\0&gt;;	13;0050000
&lt;bisb\0\0\0\0&gt;;		13;0150000
&lt;add\0\0\0\0\0&gt;;	13;0060000
&lt;sub\0\0\0\0\0&gt;;	13;0160000

/ branch

&lt;br\0\0\0\0\0\0&gt;;	06;0000400
&lt;bne\0\0\0\0\0&gt;;	06;0001000
&lt;beq\0\0\0\0\0&gt;;	06;0001400
&lt;bge\0\0\0\0\0&gt;;	06;0002000
&lt;blt\0\0\0\0\0&gt;;	06;0002400
&lt;bgt\0\0\0\0\0&gt;;	06;0003000
&lt;ble\0\0\0\0\0&gt;;	06;0003400
&lt;bpl\0\0\0\0\0&gt;;	06;0100000
&lt;bmi\0\0\0\0\0&gt;;	06;0100400
&lt;bhi\0\0\0\0\0&gt;;	06;0101000
&lt;blos\0\0\0\0&gt;;		06;0101400
&lt;bvc\0\0\0\0\0&gt;;	06;0102000
&lt;bvs\0\0\0\0\0&gt;;	06;0102400
&lt;bhis\0\0\0\0&gt;;		06;0103000
&lt;bec\0\0\0\0\0&gt;;	06;0103000
&lt;bcc\0\0\0\0\0&gt;;	06;0103000
&lt;blo\0\0\0\0\0&gt;;	06;0103400
&lt;bcs\0\0\0\0\0&gt;;	06;0103400
&lt;bes\0\0\0\0\0&gt;;	06;0103400

/ jump/branch type

&lt;jbr\0\0\0\0\0&gt;;	35;0000400
&lt;jne\0\0\0\0\0&gt;;	36;0001000
&lt;jeq\0\0\0\0\0&gt;;	36;0001400
&lt;jge\0\0\0\0\0&gt;;	36;0002000
&lt;jlt\0\0\0\0\0&gt;;	36;0002400
&lt;jgt\0\0\0\0\0&gt;;	36;0003000
&lt;jle\0\0\0\0\0&gt;;	36;0003400
&lt;jpl\0\0\0\0\0&gt;;	36;0100000
&lt;jmi\0\0\0\0\0&gt;;	36;0100400
&lt;jhi\0\0\0\0\0&gt;;	36;0101000
&lt;jlos\0\0\0\0&gt;;		36;0101400
&lt;jvc\0\0\0\0\0&gt;;	36;0102000
&lt;jvs\0\0\0\0\0&gt;;	36;0102400
&lt;jhis\0\0\0\0&gt;;		36;0103000
&lt;jec\0\0\0\0\0&gt;;	36;0103000
&lt;jcc\0\0\0\0\0&gt;;	36;0103000
&lt;jlo\0\0\0\0\0&gt;;	36;0103400
&lt;jcs\0\0\0\0\0&gt;;	36;0103400
&lt;jes\0\0\0\0\0&gt;;	36;0103400

/ single operand

&lt;clr\0\0\0\0\0&gt;;	15;0005000
&lt;clrb\0\0\0\0&gt;;		15;0105000
&lt;com\0\0\0\0\0&gt;;	15;0005100
&lt;comb\0\0\0\0&gt;;		15;0105100
&lt;inc\0\0\0\0\0&gt;;	15;0005200
&lt;incb\0\0\0\0&gt;;		15;0105200
&lt;dec\0\0\0\0\0&gt;;	15;0005300
&lt;decb\0\0\0\0&gt;;		15;0105300
&lt;neg\0\0\0\0\0&gt;;	15;0005400
&lt;negb\0\0\0\0&gt;;		15;0105400
&lt;adc\0\0\0\0\0&gt;;	15;0005500
&lt;adcb\0\0\0\0&gt;;		15;0105500
&lt;sbc\0\0\0\0\0&gt;;	15;0005600
&lt;sbcb\0\0\0\0&gt;;		15;0105600
&lt;tst\0\0\0\0\0&gt;;	15;0005700
&lt;tstb\0\0\0\0&gt;;		15;0105700
&lt;ror\0\0\0\0\0&gt;;	15;0006000
&lt;rorb\0\0\0\0&gt;;		15;0106000
&lt;rol\0\0\0\0\0&gt;;	15;0006100
&lt;rolb\0\0\0\0&gt;;		15;0106100
&lt;asr\0\0\0\0\0&gt;;	15;0006200
&lt;asrb\0\0\0\0&gt;;		15;0106200
&lt;asl\0\0\0\0\0&gt;;	15;0006300
&lt;aslb\0\0\0\0&gt;;		15;0106300
&lt;jmp\0\0\0\0\0&gt;;	15;0000100
&lt;swab\0\0\0\0&gt;;		15;0000300

/ jsr

&lt;jsr\0\0\0\0\0&gt;;	07;0004000

/ rts

&lt;rts\0\0\0\0\0&gt;;	010;000200

/ simple operand

&lt;sys\0\0\0\0\0&gt;;	011;104400

/ flag-setting

&lt;clc\0\0\0\0\0&gt;;	01;0000241
&lt;clv\0\0\0\0\0&gt;;	01;0000242
&lt;clz\0\0\0\0\0&gt;;	01;0000244
&lt;cln\0\0\0\0\0&gt;;	01;0000250
&lt;sec\0\0\0\0\0&gt;;	01;0000261
&lt;sev\0\0\0\0\0&gt;;	01;0000262
&lt;sez\0\0\0\0\0&gt;;	01;0000264
&lt;sen\0\0\0\0\0&gt;;	01;0000270

/ floating point ops

&lt;cfcc\0\0\0\0&gt;;		01;170000
&lt;setf\0\0\0\0&gt;;		01;170001
&lt;setd\0\0\0\0&gt;;		01;170011
&lt;seti\0\0\0\0&gt;;		01;170002
&lt;setl\0\0\0\0&gt;;		01;170012
&lt;clrf\0\0\0\0&gt;;		15;170400
&lt;negf\0\0\0\0&gt;;		15;170700
&lt;absf\0\0\0\0&gt;;		15;170600
&lt;tstf\0\0\0\0&gt;;		15;170500
&lt;movf\0\0\0\0&gt;;		12;172400
&lt;movif\0\0\0&gt;;		14;177000
&lt;movfi\0\0\0&gt;;		05;175400
&lt;movof\0\0\0&gt;;		14;177400
&lt;movfo\0\0\0&gt;;		05;176000
&lt;addf\0\0\0\0&gt;;		14;172000
&lt;subf\0\0\0\0&gt;;		14;173000
&lt;mulf\0\0\0\0&gt;;		14;171000
&lt;divf\0\0\0\0&gt;;		14;174400
&lt;cmpf\0\0\0\0&gt;;		14;173400
&lt;modf\0\0\0\0&gt;;		14;171400
&lt;movie\0\0\0&gt;;		14;176400
&lt;movei\0\0\0&gt;;		05;175000
&lt;ldfps\0\0\0&gt;;		15;170100
&lt;stfps\0\0\0&gt;;		15;170200
&lt;fr0\0\0\0\0\0&gt;;	24;000000
&lt;fr1\0\0\0\0\0&gt;;	24;000001
&lt;fr2\0\0\0\0\0&gt;;	24;000002
&lt;fr3\0\0\0\0\0&gt;;	24;000003
&lt;fr4\0\0\0\0\0&gt;;	24;000004
&lt;fr5\0\0\0\0\0&gt;;	24;000005

/ 11/45 operations

&lt;als\0\0\0\0\0&gt;;	30;072000
&lt;alsc\0\0\0\0&gt;;		30;073000
&lt;mpy\0\0\0\0\0&gt;;	30;070000
.if eae-1
&lt;mul\0\0\0\0\0&gt;;	30;070000
&lt;div\0\0\0\0\0&gt;;	30;071000
&lt;ash\0\0\0\0\0&gt;;	30;072000
&lt;ashc\0\0\0\0&gt;;		30;073000
.endif
&lt;dvd\0\0\0\0\0&gt;;	30;071000
&lt;xor\0\0\0\0\0&gt;;	07;074000
&lt;sxt\0\0\0\0\0&gt;;	15;006700
&lt;mark\0\0\0\0&gt;;		11;006400
&lt;sob\0\0\0\0\0&gt;;	31;077000

/ specials

&lt;.byte\0\0\0&gt;;		16;000000
&lt;.even\0\0\0&gt;;		20;000000
&lt;.if\0\0\0\0\0&gt;;	21;000000
&lt;.endif\0\0&gt;;		22;000000
&lt;.globl\0\0&gt;;		23;000000
&lt;.text\0\0\0&gt;;		25;000000
&lt;.data\0\0\0&gt;;		26;000000
&lt;.bss\0\0\0\0&gt;;		27;000000
&lt;.comm\0\0\0&gt;;		32;000000

ebsymtab:


start:
	sys	signal; 2; 1
	ror	r0
	bcs	1f
	sys	signal; 2; aexit
1:
	mov	sp,r5
	mov	(r5)+,r0
	cmpb	*2(r5),$&#39;-
	bne	1f
	tst	(r5)+
	dec	r0
	br	2f
1:
	clr	unglob
2:
	movb	r0,nargs
	mov	r5,curarg
	jsr	r5,fcreat; a.tmp1
	movb	r0,pof
	jsr	r5,fcreat; a.tmp2
	movb	r0,fbfil
	jsr	pc,setup
	jmp	go

setup:
	mov	$symtab,r1
1:
	clr	r3
	mov	$8,r2
	mov	r1,-(sp)
2:
	movb	(r1)+,r4
	beq	2f
	add	r4,r3
	swab	r3
	sob	r2,2b
2:
	clr	r2
	div	$hshsiz,r2
	ashc	$1,r2
	add	$hshtab,r3
4:
	sub	r2,r3
	cmp	r3,$hshtab
	bhi	3f
	add	$2*hshsiz,r3
3:
	tst	-(r3)
	bne	4b
	mov	(sp)+,r1
	mov	r1,(r3)
	add	$12.,r1
	cmp	r1,$ebsymtab
	blo	1b
	rts	pc

/overlay buffer
inbuf	= setup
.	=inbuf+512.

