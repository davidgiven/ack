        # $Header$
.globl	.trp

.trp:
	movl	r0,-(sp)
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
	movl	r0,-(sp)	# trapnumber on stack
	movl	.trppc,r0
	beql	L2		#is there a user defined traphandler?
	clrl	.trppc
	jsb	(r0)		#if so, call it
	tstl	(sp)+
	popr	$017776		#restore registers
	movl	(sp)+,r0
	rsb
L2:
	bpt
