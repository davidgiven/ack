.define .rol4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Rotate 4 bytes left
! Expects in de-reg:	number of rotates
! Expects on stack:	operand
! Yields on stack:	result

.rol4:	pop h
	shld .retadr

	pop h		! low-order bytes of operand
	pop b		! high order bytes of operand

	mov a,e
	ani 31
	jz 2f
	mov e,a

	mov a,b
1:	ral
	mov a,l
	ral
	mov l,a
	mov a,h
	ral
	mov h,a
	mov a,c
	ral
	mov c,a
	mov a,b
	ral
	mov b,a

	dcr e
	jnz 1b		! keep looping

2:	push b
	push h

	lhld .retadr
	pchl
