	# $Header$
.globl .strlb
	# Store the value that is on the stack in fp.
	# Compute ap.
.strlb:
	movl	(sp)+,r3
	movl	(sp)+,fp
	pushl	r3
	movl	$20,ap		# Compute argumentbase from local base.
				# Distance is 5 longwords + the number of
				# registers saved.
	movl	$1,r3		# Now check register save mask
	movl	$12,r2
L1:
	bitl	r3,6(fp)
	beql	L2
	addl2	$4,ap		# Add 1 longword for each register saved
L2:
	ashl	$1,r3,r3
	sobgeq	r2,L1

	extzv	$14,$2,6(fp),r3	# Now find out about the stack alignment
				# between fp and ap
	addl2	r3,ap		# add alignment
	addl2	fp,ap
	rsb
