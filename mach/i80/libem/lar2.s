.define .lar2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Load array element, descriptor contains 2-bytes integers
! Expects on stack:	pointer to array descriptor
!			index
!			base address
! Yields on stack:	array element
! Adapted from .aar2 and .loi

.lar2:
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
	dad b		! hl= load pointer
	xra a		! clear carry bit
	mov a,b		! divide bc by 2
	rar
	mov b,a
	mov a,c
	rar
	mov c,a
	jnc 1f
	
! for 1 byte array element only:
	mov a,c		! trap if bc odd and <>1
	ora b
	cnz eoddz
	dcx h
	mov e,m
	mvi d,0
	push d
	jmp 2f

1:	dcx h
	mov d,m
	dcx h
	mov e,m
	push d
	dcx b
	mov a,b
	ora c
	jnz 1b

2:	
	lhld .retadr1
	pchl
