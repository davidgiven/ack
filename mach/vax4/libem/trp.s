        # $Header$
.globl	.trp

.trp:
	pushl	r0
	movl	8(sp),r0
	movl	4(sp),8(sp)
	movl	(sp)+,(sp)
	cmpl	r0,$16
	bgequ	L1		# a trapnumber >= 16 cannot be ignored
	bbc	r0,.trpim,L1
	movl	(sp)+,r0
	rsb
L1:
	pushr	$017776		# save registers
	pushl	r0		# trapnumber on stack
	movl	.trppc,r0
	beql	L2		# is there a user defined traphandler?
	clrl	.trppc
	calls	$1,(r0)		# if so, call it
	popr	$017776		# restore registers
	movl	(sp)+,r0
	rsb

.set	write,4

L2:
	# Put the (octal) trapnumber in the zeroes in Lemes[].

	movl	$Lemesend-1,r1	# Addres after last '0'.
	movl	$5,r2		# Max number of digits.
	movl	(sp),r0		# Trap number in r0.
L3:
	bicw2	$0177770,r0	# Lower 3 bits form lower octal digit.
	bisb2	r0,-(r1)	# Put them in the '0'.
	ashl	$-3,r0,r0	# Shift the 3 bits off.
	sobgtr	r2,L3
	movl	ap,r2
	movl	$Lwr,ap
	chmk	$write
	movl	r2,ap
	bpt
.data
Lemes:
.byte 'E,'r,'r,' ,'0,'0,'0,'0,'0,0xa
Lemesend:
.align 2
Lwr:
.long 3			# 3 arguments.
.long 2			# File descriptor 2.
.long Lemes		# Address of character buffer.
.long Lemesend - Lemes	# Number of characters to write.
