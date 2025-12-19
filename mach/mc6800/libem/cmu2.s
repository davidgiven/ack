.define Cmu2
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine compares on two signed integers.
!   -1  if S < T,
!    0  if S = T,
!    1  if S > T.

! AccAB: T
! Stack:
! +0:	return address
! +2:	S

Cmu2:
	tsx
	subb 3,x	! T - S
	sbca 2,x
	bcs 2f		! S > T
	bhi 3f		! S < T
	tstb
	bne 3f		! S != T  (i.e S < T)
			! S == T, AccAB already zero.
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
