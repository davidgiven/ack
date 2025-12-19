.define Sli2
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


Sli2:
	tsx
	ldx 0,x
	ins
	ins
	stab <TMP+1	! shift count
	pula
	pulb
	beq 2f
1:	aslb
	rola		! shift left
	dec TMP+1
	bne 1b
2:	jmp 0,x
