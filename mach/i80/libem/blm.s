.define .blm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Block move
! Expects in de-reg:	size of block
! Expects on stack:	destination address
!			source address

.blm:	pop h
	shld .retadr

	pop h		! hl = destination address
	pop b		! bc = source address

1:	ldax b
	mov m,a
	inx b
	inx h
	dcx d
	mov a,d
	ora e
	jnz 1b

	lhld .retadr
	pchl

