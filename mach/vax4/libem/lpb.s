.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.lpb

        ! $Id$
	! special version to work with A68S, by CHL
        ! compute AB, given LB
.lpb:
	cmpl	r0,fp
	bneq	I1
	moval	4(ap),r0
	rsb
I1:
        movl    4(r0),r1        ! mask for saved registers
        addl2   $24,r0          ! position of AB if no registers saved
        movl    $16,r2          ! position of first bit to test
I2:
        subl3   r2,$28,r3       ! remaining size of mask
        ffs     r2,r3,r1,r2     ! find first bit set in mask
        beql    I3              ! no more bits set
        addl2   $4,r0           ! for each saved register
        incl    r2
        jbr     I2
I3:
        extzv   $30,$2,r1,r2    ! Now find out about the stack alignment
                                ! between fp and ap
        addl2   r2,r0           ! add alignment
        rsb
