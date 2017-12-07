.define .ror4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Rotate 4 bytes right
! Expects in de-reg:	number of rotates
! Expects on stack:	operand
! Yields on stack:	result

.ror4:	pop h
	shld .retadr
	mov h,b
	mov l,c
	shld .bcreg

	pop h		! low-order bytes of operand
	pop b		! high order bytes of operand

	mov a,e
	ani 31
	jz 2f
	mov e,a

	mov a,l
1:	rar
	mov a,b
	rar
	mov b,a
	mov a,c
	rar
	mov c,a
	mov a,h
	rar
	mov h,a
	mov a,l
	rar
	mov l,a

	dcr e
	jnz 1b		! keep looping

2:	push b
	push h

	lhld .bcreg
	mov b,h
	mov c,l
	lhld .retadr
	pchl
