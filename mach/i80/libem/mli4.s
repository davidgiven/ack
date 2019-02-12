.define .mli4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! 32 bits signed and unsigned integer multiply routine
! Expects operands on stack
! Yields product on stack

.mli4:	pop h
	shld .retadr

	pop h			! store multiplier
	shld block1
	pop h
	shld block1+2
	pop h			! store multiplicand
	shld block2
	pop h
	shld block2+2
	lxi h,0
	shld block3		! product = 0
	shld block3+2
	lxi b,0
lp1:	lxi h,block1
	dad b
	mov a,m			! get next byte of multiplier
	mvi b,8
lp2:	rar
	jnc 2f
	lhld block2		! add multiplicand to product
	xchg
	lhld block3
	dad d
	shld block3
	lhld block2+2
	jnc 1f
	inx h
1:	xchg
	lhld block3+2
	dad d
	shld block3+2

2:	lhld block2		! shift multiplicand left
	dad h
	shld block2
	lhld block2+2
	jnc 3f
	dad h
	inx h
	jmp 4f
3:	dad h
4:	shld block2+2

	dcr b
	jnz lp2

	inr c
	mov a,c
	cpi 4
	jnz lp1

	lhld block3+2
	push h
	lhld block3
	push h

	lhld .retadr
	pchl
