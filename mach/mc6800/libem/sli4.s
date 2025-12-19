.define Sli4
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine shifts a signed or unsigned interger to the
! left n times.
! N is in register B.
! The returned value is in registerpair AB.


Sli4:
	tsx
	tstb 		! shift count
	beq 9f
2:	asl 5,x
	rol 4,x
	rol 3,x
	rol 2,x
	decb
	bne 2b
9:
	ldx 0,x
	ins
	ins
	jmp 0,x


