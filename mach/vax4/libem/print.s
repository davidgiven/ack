.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  printd
.define  printc

        ! $Header$

printd:
	clrl    r1
	ediv    $10,r0,r0,r1
	beql    I1
	pushl   r1
	jsb     printd
	movl    (sp)+,r1
I1:
	addb3   $'0',r1,r0

printc:
	movb    r0,Ich
	movl	ap,r2
	movl    $I9,ap
	chmk    $4
	movl	r2,ap
	rsb

	.sect .rom
I9:
	.data4   3
	.data4   2
	.data4   Ich
	.data4   1

	.sect .data
Ich:    .data2   0
