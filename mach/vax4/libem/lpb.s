        # $Header$
.globl	.lpb

.lpb:
	cmpl	r0,fp
	bneq	L1
	moval	4(ap),r0
	rsb
L1:
	movl	fp,r1
L2:
	cmpl	12(r1),r0
	beql	L3
	movl	12(r1),r1
	jbr	L2
L3:
	addl3	$4,8(r1),r0	# Argument Base = ap + 4
	rsb
