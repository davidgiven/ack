.define ___Get_PC, ___Set_PC, ___u_LiB
.sect .text; .sect .rom; .sect .data

! $Id$

! This is the machine-dependant part of the ACK debugger support.
! ___Get_PC takes a frame pointer (local base) argument and returns the
!	    return address from this frame.
! ___Set_PC takes a frame pointer (local base) argument plus a return
!	    address and sets the return address of this frame to the
!	    return address supplied.
! ___u_LiB  has a special register save mask: it also saves the
!	    scratch registers.

.sect .text

___Get_PC:
	.data2 00
	movl	4(ap),r0
	beql	1f
	movl	16(r0),r0
1:
	ret

___Set_PC:
	.data2 00
	movl	4(ap),r0
	beql	1f
	movl	8(ap),16(r0)
1:
	ret

___u_LiB:
	.data2 07777
	jmp ___uX_LiB+2
