.define .set2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Create 16 bits set with one bit on
! Expects in de-reg:	bit number
! Yields in de-reg:	resulting set

.set2:	mov a,d		!trap if bit number >= 16
	ora a
	cnz eset
	mov a,e
	cpi 16
	cnc eset

	pop h
	shld .retadr
	mov a,e
	ani 7
	mov d,a
	mvi a,1
1:	dcr d
	jm 2f
	rlc
	jmp 1b
2:	mov d,a
	mov a,e
	ani 8
	jnz 3f		! jump if bit 3 is set

	mov e,d
	mvi d,0
	jmp 4f

3:	mvi e,0

4:	lhld .retadr
	pchl
