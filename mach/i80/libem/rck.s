.define .rck
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Range check
! Expects on stack: address of range check descriptor
!		    index
! Yields index on stack unchanged
! Causes a trap if index is out of bounds

.rck:	pop h
	shld .retadr
	mov h,b
	mov l,c
	shld .bcreg

	pop h			! hl = return address
	pop d			! de = index
	mov c,m			! bc = lower bound
	inx h
	mov b,m
	inx h
	mov a,d
	xri b
	jm 1f			! jump if index and l.b. have different signs
	mov a,e
	sub c
	mov a,d
	sbb b
	jmp 2f

1:	xri b			! now a = d again
2:	cm erange		! trap if index too small

	mov c,m
	inx h
	mov b,m
	mov a,d
	xri b
	jm 1f			! jump if index and u.b. have different signs
	mov a,c
	sub e
	mov a,b
	sbb d
	jmp 2f

1:	xri d			! now a = b
2:	cm erange		! trap if index is too large

	lhld .bcreg
	mov b,h
	mov c,l
	lhld .retadr
	pchl
