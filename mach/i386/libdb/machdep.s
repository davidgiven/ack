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
!	    ___u_LiB is the solution chosen for the Intel 80386 back-end.
!	    Other back-ends could choose different solutions.

! The local base of ___uX_LiB is kind of special: because of ___u_LiB,
! the "interesting" return address of this particular stack-frame is found
! in a different place than the one of a normal stack-frame:
! the "interesting" return address is in fact the return address of
! ___u_LiB. Therefore, we save the local base of ___uX_LiB in sv_bp.

.sect .data
sv_bp:
	.data4 0

.sect .text
___Get_PC:
	mov	ebx,esp
	mov	eax,4(ebx)
	or	eax,eax
	je	1f
	cmp	(sv_bp),eax
	je	2f
	mov	eax,4(eax)
1:
	ret
2:
	mov	eax,28(eax)
	ret

___Set_PC:
	mov	ebx,esp
	mov	eax,4(ebx)
	mov	ebx,8(ebx)
	or	eax,eax
	je	1f
	cmp	(sv_bp),eax
	je	2f
	mov	4(eax),ebx
1:
	ret
2:
	mov	28(eax),ebx
	ret

___u_LiB:
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	(sv_bp)
	mov	eax,esp
	sub	eax,8
	mov	(sv_bp),eax	! this is the LB of __uX_LiB
	call	___uX_LiB
	pop	(sv_bp)
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	ret
