.define Cmi
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine compares on two integers.
!   -1  if S < T,
!    0  if S = T,
!    1  if S > T.

! AccAB: T
! stack:
! +0:	return address
! +2:	S


Cmi:
	tsx
	subb 3,x	! T - S
	sbca 2,x
	blt 1f		! S > T
	bgt 3f		! S < T
	tstb
	bne 3f		! S != T  (i.e S < T)
			! S == T, AccAB already zero.
2:
	ldx 0,x
	ins
	ins
	ins
	ins
	jmp 0,x
1:
	ldab #1		! S > T
	clra		! AB becomes 1
	bra 2b
3:
	ldab #0xff	! S < T
	tba		! AB becomes -1
	bra 2b
