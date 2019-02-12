.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.define .proword, .probyte
.define .pro0, .pro2, .pro4

! BC is used as the frame pointer, but HL and DE can be corrupted.

.proword:
	lhld .fp
	xthl
	mov e, m
	inx h
	mov d, m
	inx h
	jmp .pron

.probyte:
	lhld .fp
	xthl
	mvi d, 0xff
	mov e, m
	inx h
	jmp .pron

.pro4:
	lhld .fp
	xthl
	lxi d, -4
	jmp .pron

.pro2:
	lhld .fp
	xthl
	lxi d, -2
	jmp .pron

.pro0:
	lhld .fp
	xthl
	lxi d, 0
	! fall through
.pron:
	mov b, h
	mov c, l
	! Copy the current stack pointer to the frame pointer.
	lxi h, 0
	dad sp
	shld .fp
	! Calculate the new stack pointer.
	dad d
	sphl
	mov h, b
	mov l, c
	pchl

