.define .exg
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Exchange top bytes of stack
! Expects in de-registers the number of bytes to be exchanged.

.exg:	mov a,e
	rar
	cc eoddz		!trap if numer of bytes is odd
	pop h
	shld .retadr

	lxi h,0
	dad sp
	mov b,h
	mov c,l			!now bc points to first operand
	dad d			!and hl to the second
	push d			!place number of bytes on top of stack
1:	mov d,m
	ldax b
	mov m,a
	mov a,d
	stax b
	xthl			!caused by a lack of registers
	dcx h			!decrement top of stack
	mov a,h
	ora l
	xthl
	inx h
	inx b
	jnz 1b

	pop d
	lhld .retadr
	pchl
