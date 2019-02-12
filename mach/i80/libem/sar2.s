.define .sar2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Store array element, descriptor contains 2-bytes integers
! Expects on stack:	pointer to array descriptor
!			index
!			base address
!			array element
! Adapted from .aar2 and .sti

.sar2:
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
	xra a
	mov a,b
	rar
	mov b,a
	mov a,c
	rar
	mov c,a		! bc = word count
	jnc 1f

! if 1 byte array element only:
	mov a,c		! trap if bc odd and <>1
	ora b
	cnz eoddz
	pop d
	mov m,e
	jmp 2f

1:	pop d
	mov m,e
	inx h
	mov m,d
	inx h
	dcx b
	mov a,b
	ora c
	jnz 1b

2:	
	lhld .retadr1
	pchl
