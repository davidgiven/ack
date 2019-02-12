.define .aar2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Load address of array element, decriptor contains 2-bytes integers
! Expects on stack:	pointer to array descriptor
! 			index
!			base address
! Yields on stack:	address of array element

.aar2:
	pop h
	shld .retadr1

	pop h		! hl = pointer to descriptor
        pop d           ! de = index
	mov a,e		! bc = index - lower bound
	sub m
	inx h
	mov c,a
	mov a,d
	sbb m
	inx h
	mov b,a
	push b		! first operand to multiply
	inx h
	inx h
	mov c,m		! bc = size
	inx h
	mov b,m
	push b		! second operand to multiply
	call .mli2	! de = size * (index - lower bound)
	pop h		! hl = base address
	dad d		! hl = address of array[index]
	push h

	lhld .retadr1
	pchl
