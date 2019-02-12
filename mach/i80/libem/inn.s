.define .inn
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Any size bit test on set.
! Expects in de-reg:	size of set (in bytes)
! Expects on stack:	bit number
!			set to be tested
! Yields in de-reg.:	0 if bit is reset or bit number out of range
!			1 if bit is set

.inn:	pop h
	shld .retadr

	pop h
	xchg		!hl = size, de = bit number
	mov a,d		!test if bit number is negative
	ral
	jc 3f
	mov a,e
	ani 7
	mov b,a		!save bits 0-2 of bit number in b-reg
	mvi c,3
1:	xra a
	mov a,d		!shift bit number right 3 times
	rar
	mov d,a
	mov a,e
	rar
	mov e,a
	dcr c
	jnz 1b

	mov a,l		!test if bit number is small enough
	sub e
	mov a,h
	sbb d
	jc 3f
	xchg
	dad sp
	xchg
	ldax d		!a-register = wanted byte
2:	dcr b		!dcr doesn't affect carry bit
	jm 4f
	rar
	jmp 2b

3:	xra a		!return 0 if bit number out of range
4:	ani 1
	mov e,a
	mvi d,0
	dad sp
	sphl

	lhld .retadr
	pchl

