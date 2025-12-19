.define Cmi4
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine compares on fourbyte unsigned integers.
!    -1   if S < T,
!     0   if S = T,
!     1   if S > T.

! Stack
! +0 to +1 : return address
! +2 to +5 : T
! +6 to +9 : S

Cmi4:
	tsx
	ldab 6,x
	cmpb 2,x
	blt 2f
	bgt 3f
	ldab 7,x
	cmpb 3,x
	bcs 2f
	bhi 3f
	ldab 8,x
	cmpb 4,x
	bcs 2f
	bhi 3f
	ldaa 9,x
	cmpa 5,x
	bcs 2f
	bhi 3f
! equal
1:	! AccA,B already 0
	ldx 0,x
	ins
	ins
	ins
	ins
	ins
	ins
	ins
	ins
	ins
	ins
	jmp 0,x
! S < T
2:
	ldab #0xff
	tba
	bra 1b
! S > T
3:
	ldab #1
	clra
	bra 1b
