.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.define .proword, .probyte
.define .pro0, .pro2, .pro4

! BC is used as the frame pointer, but HL and DE can be corrupted.

.proword:
	pop h
	push b
	mov e, m
	inx h
	mov d, m
	inx h
	jmp .pron

.probyte:
	pop h
	push b
	mvi d, 0xff
	mov e, m
	inx h
	jmp .pron

.pro4:
	pop h
	push b
	lxi d, -4
	jmp .pron

.pro2:
	pop h
	push b
	lxi d, -2
	jmp .pron

.pro0:
	pop h
	push b
	lxi d, 0
	! fall through
.pron:
	shld .retadr
	! Copy the current stack pointer to BC.
	lxi h, 0
	dad sp
	mov b, h
	mov c, l
	! Calculate the new stack pointer.
	dad d
	sphl
	lhld .retadr
	pchl

