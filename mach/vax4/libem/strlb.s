.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .strlb

	! $Id$
	! revised version by CHL
	! Store the value that is on the stack in fp.
	! Compute ap.
.strlb:
	movl	(sp)+,r3
	movl	(sp)+,fp
	pushl	r3
	movl	$20,ap		! Compute argumentbase from local base.
				! Distance is 5 longwords + the number of
				! registers saved.
	movl	4(fp),r3	! mask for saved registers
	movl	$16,r2		! position of first bit to test
I4:
        subl3   r2,$28,r1       ! remaining size of mask
        ffs     r2,r1,r3,r2     ! find first bit set in mask
        beql    I5              ! no more bits set
        addl2   $4,ap           ! for each saved register
        incl    r2
        jbr     I4
I5:
	extzv	$14,$2,6(fp),r3	! Now find out about the stack alignment
				! between fp and ap
	addl2	r3,ap		! add alignment
	addl2	fp,ap
	rsb
