.define Sru2
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine shifts a unsigned interger to the
! right n times.
! N is in register B.
! The returned value is in registerpair AB.


Sru2:
	tsx
	ldx 0,x
	ins
	ins
	stab <TMP+1	! shift count
	pula
	pulb
	beq 2f		! zero shift, return input
1:	lsra
	rorb		! shift right
	dec TMP+1
	bne 1b
2:	jmp 0,x


