.define .ngi4

! Exchange 32 bits integer by its two's complement
! Expects operand on stack
! Yields result on stack

.ngi4:	pop d
	lxi h,0
	dad sp
	xra a
	sub m
	mov m,a
	inx h
	mvi a,0
	sbb m
	mov m,a
	inx h
	mvi a,0
	sbb m
	mov m,a
	inx h
	mvi a,0
	sbb m
	mov m,a
	push d
	ret
