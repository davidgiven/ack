.globl ___Get_PC, ___Set_PC

! $Header$

! This is the machine-dependant part of the ACK debugger support.
! ___Get_PC takes a frame pointer (local base) argument and returns the
!	    return address from this frame.
! ___Set_PC takes a frame pointer (local base) argument plus a return
!	    address and sets the return address of this frame to the
!	    return address supplied.

___Get_PC:
	ld	[%l0],%o0
	tst	%o0
	beq	3f
	nop
	cmp	%o0,%l1
	bne	1f
	nop
	add	%i7, 8, %o0
3:
	retl
	nop
1:
	ta	3
	add	%l1,%g0,%o1
2:	add	%o1,7,%o2
	andn	%o2,7,%o2
	ld	[%o2+4],%o1
	cmp	%o0,%o1
	bne	2b
	nop
	ld	[%o2+0x3c],%o0
	inc	8,%o0
	retl
	nop

___Set_PC:
	ld	[%l0],%o0
	tst	%o0
	beq	3f
	nop
	cmp	%o0,%l1
	bne	1f
	nop
	ld	[%l0+4],%i7
	dec	8,%i7
3:
	retl
	nop
1:
	ta	3
	add	%l1,%g0,%o1
2:	add	%o1,7,%o2
	andn	%o2,7,%o2
	ld	[%o2+4],%o1
	cmp	%o0,%o1
	bne	2b
	nop
	ld	[%l0+4],%o0
	dec	8,%o0
	st	%o0,[%o2+0x3c]
	retl
	nop
