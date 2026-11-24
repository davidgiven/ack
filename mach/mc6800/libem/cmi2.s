.define Cmi2
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine compares two signed 16-bit integers.
!   -1  if S < T,
!    0  if S = T,
!    1  if S > T.
!
! AccAB: T
! Stack:
! +0:	return address
! +2:	S

Cmi2:
	tsx
	subb 3,x	! T - S
	sbca 2,x
	blt 2f		! S > T
	bgt 3f		! S < T
	tstb
	bne 3f		! positive difference in the low byte
			! AccAB is already zero for equal operands
1:
	ldx 0,x
	ins
	ins
	ins
	ins
	jmp 0,x
2:
	ldab #1		! S > T
	clra		! AccAB = 1
	bra 1b
3:
	ldab #0xff	! S < T
	tba		! AccAB = -1
	bra 1b
