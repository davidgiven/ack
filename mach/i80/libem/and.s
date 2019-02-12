.define .and
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Any size logical-'and'.
! Expects:	size in de-registers
!		operands on stack
! Yields:	result on stack

.and:	pop h
	shld .retadr

	lxi h,0
	dad sp
	mov c,l
	mov b,h		!now bc points to top of first operand
	dad d		!and hl points to top of second perand
	push h		!this will be the new stackpointer
1:	ldax b
	ana m
	mov m,a
	inx h
	inx b
	dcx d
	mov a,e
	ora d
	jnz 1b

	pop h
	sphl

	lhld .retadr
	pchl
