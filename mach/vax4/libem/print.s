.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  printd
.define  printc

        ! $Id$

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
	movb	r0,Ich
	pushl	$1
	pushab	Ich
	pushl	$2
	calls	$3,WRITE
	rsb

	.sect .data
Ich:    .data1   0
