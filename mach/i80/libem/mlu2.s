.define .mlu2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! 16 bits unsigned multiply routine
! Expects operands on stack
! Yields result in de-registers
! This routine could also be used for signed integers, but it won't
! because there is a more clever one just for signed integers.

.mlu2:
	pop h
	shld .retadr

	pop b		! bc = multiplier
	pop d		! de = multiplicand
	lxi h,0		! hl = product

1:	mov a,b		! if multiplier = 0 then finished
	ora c
	jz 3f

	xra a		! reset carry
	mov a,b		! shift multiplier right
	rar
	mov b,a
	mov a,c
	rar
	mov c,a

	jnc 2f		!if carry set: add multiplicand to product
	dad d

2:	xchg		! shift multiplicand left
	dad h
	xchg
	jmp 1b		! keep looping

3:	xchg		! de becomes product

	lhld .retadr
	pchl
