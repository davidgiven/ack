        # $Header$
	# special version to work with A68S, by CHL
.globl	.lpb
        # compute AB, given LB
.lpb:
	cmpl	r0,fp
	bneq	L1
	moval	4(ap),r0
	rsb
L1:
        movl    4(r0),r1        # mask for saved registers
        addl2   $24,r0          # position of AB if no registers saved
        movl    $16,r2          # position of first bit to test
L2:
        subl3   r2,$28,r3       # remaining size of mask
        ffs     r2,r3,r1,r2     # find first bit set in mask
        beql    L3              # no more bits set
        addl2   $4,r0           # for each saved register
        incl    r2
        jbr     L2
L3:
        extzv   $30,$2,r1,r2    # Now find out about the stack alignment
                                # between fp and ap
        addl2   r2,r0           # add alignment
        rsb
