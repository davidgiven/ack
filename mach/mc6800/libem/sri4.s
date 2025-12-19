.define Sri4
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine shifts a signed or signed interger to the
! right n times.
! N is in register B.
! The returned value is in registerpair AB.


Sri4:
	tsx
	tstb 		! shift count
	beq 9f
2:	asr 2,x
	ror 3,x
	ror 4,x
	ror 5,x
	decb
	bne 2b
9:
	ldx 0,x
	ins
	ins
	jmp 0,x


