.define .sbi4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Subtract two 32 bits signed or unsigned integers.
! Expects operands on stack
! Yields result on stack

.sbi4:	
	pop h
	shld .retadr
	mov h,b
	mov l,c
	shld .bcreg

	lxi h,0
	dad sp		!now hl points to the first operand
	mov d,h
	mov e,l
	inx d
	inx d
	inx d
	inx d		!and de points to the second.
	mvi b,4
	xra a
1:	ldax d
	sbb m
	stax d
	inx d
	inx h
	dcr b
	jnz 1b
	sphl

	lhld .bcreg
	mov b,h
	mov c,l
	lhld .retadr
	pchl
