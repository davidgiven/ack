.define .dup
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Duplicate top bytes of stack
! Expects in de-registers: number of bytes to duplicate

.dup:	mov a,e		!trap if number is odd
	rar
	cc eoddz

	pop h
	shld .retadr
	mov h,b
	mov l,c
	shld .bcreg

	mov h,d
	mov l,e
	dad sp
1:	dcx h
	mov b,m
	dcx h
	mov c,m
	push b
	dcx d
	dcx d		!number of bytes must be a word-multiple i.e. even
	mov a,d
	ora e
	jnz 1b

	lhld .bcreg
	mov b,h
	mov c,l
	lhld .retadr
	pchl
