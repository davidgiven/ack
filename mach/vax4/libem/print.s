        # $Header$
.globl  printd
.globl  printc

printd:
	clrl    r1
	ediv    $10,r0,r0,r1
	beql    L1
	pushl   r1
	jsb     printd
	movl    (sp)+,r1
L1:
	addb3   $'0,r1,r0

printc:
	movb    r0,Lch
	movl	ap,r2
	movl    $L9,ap
	chmk    $4
	movl	r2,ap
	rsb

	.data
L9:
	.long   3
	.long   2
	.long   Lch
	.long   1
Lch:     .word   0
