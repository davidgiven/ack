.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.trp
        ! $Header$

.trp:
	pushl	r0
	movl	8(sp),r0
	movl	4(sp),8(sp)
	movl	(sp)+,(sp)
	cmpl	r0,$16
	bgequ	I1		! a trapnumber >= 16 cannot be ignored
	bbc	r0,.trpim,I1
	movl	(sp)+,r0
	rsb
I1:
	pushr	$017776		! save registers
	pushl	r0		! trapnumber on stack
	movl	.trppc,r0
	beql	I2		! is there a user defined traphandler?
	clrl	.trppc
	calls	$1,(r0)		! if so, call it
	popr	$017776		! restore registers
	movl	(sp)+,r0
	rsb

I2:
	! Put the (octal) trapnumber in the zeroes in Iemes[].

	movl	$Iemesend-9,r1	! Addres after last '0'.
	movl	$5,r2		! Max number of digits.
	movl	(sp),r0		! Trap number in r0.
I3:
	bicw3	$0177770,r0,r3	! Lower 3 bits form lower octal digit.
	bisb2	r3,-(r1)	! Put them in the '0'.
	ashl	$-3,r0,r0	! Shift the 3 bits off.
	sobgtr	r2,I3
	pushl	$Iemesend-Iemes
	pushab	Iemes
	pushl	$2
	calls	$3,WRITE
	bpt
.sect .data
Iemes:
.data1 'E','M',' ','t','r','a','p',' ','0','0','0','0','0',' ','(','o','c','t','a','l',')',0xa
Iemesend:
.align 2
