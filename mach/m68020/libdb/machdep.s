.define ___Get_PC, ___Set_PC, ___u_LiB
.sect .text; .sect .rom; .sect .data

! $Id$

! This is the machine-dependant part of the ACK debugger support.
! ___Get_PC takes a frame pointer (local base) argument and returns the
!	    return address from this frame.
! ___Set_PC takes a frame pointer (local base) argument plus a return
!	    address and sets the return address of this frame to the
!	    return address supplied.
! ___u_LiB  saves the scratch registers and then calls ___uX_LiB;
!	    when ___uX_LiB returns, the scratch registers are restored.
!	    ___u_LiB is the solution chosen for the MC68020 back-end.
!	    Other back-ends could choose different solutions.

! The local base of ___uX_LiB is kind of special: because of ___u_LiB,
! the "interesting" return address of this particular stack-frame is found
! in a different place than the one of a normal stack-frame:
! the "interesting" return address is in fact the return address of
! ___u_LiB. Therefore, we save the local base of ___uX_LiB in sv_a6.

.sect .data
sv_a6:
	.data4 0

.sect .text
___Get_PC:
	move.l	(4,sp),d0
	beq	1f
	move.l	d0,a0
	cmp.l	(sv_a6),d0
	beq	2f
	move.l	(4,a0),d0
1:
	rts
2:
	move.l	(32,a0),d0
	rts

___Set_PC:
	move.l	(4,sp),d0
	beq	1f
	move.l	d0,a0
	cmp.l	(sv_a6),d0
	beq	2f
	move.l	(8,sp),(4,a0)
1:
	rts
2:
	move.l	(8,sp),(32,a0)
	rts

___u_LiB:
	movem.l	d0/d1/d2/a0/a1,-(sp)
	move.l	(sv_a6),-(sp)
	move.l	sp,d0
	sub.l	#8,d0
	move.l	d0,(sv_a6)	! this is the LB of __uX_LiB
	jsr (___uX_LiB)
	move.l	(sp)+,(sv_a6)
	movem.l	(sp)+,d0/d1/d2/a0/a1
	rts
