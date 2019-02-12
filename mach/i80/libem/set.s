.define .set
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Create set with one bit on
! Expects in de-reg:	size of set to be created
! Expects on stack:	bit number
! Yields on stack:	resulting set

.set:	pop h
	shld .retadr

	mov a,e
	rar
	cc eoddz	! trap if size is odd
	xchg		! hl = size of set
	pop d		! de = bit number
	mov a,e		! c = bit number in byte
	ani 7
	sta .areg	! save bit number in byte

	mvi b,3		! de = byte number
1:	xra a
	mov a,d
	rar
	mov d,a
	mov a,e
	rar
	mov e,a
	dcr b
	jnz 1b

	mov a,l		! trap if bit number is too large
	sub e
	mov a,h
	sbb d
	cc eset

	lxi b,0		! make empty set on stack
1:	push b
	dcx h
	dcx h
	mov a,l
	ora h
	jnz 1b

	lxi h,0
	dad sp
	dad d		! hl points to byte that will contain a one
	lda .areg
	mov c,a		! c = bit number in byte
	mvi a,1
1:	dcr c
	jm 2f
	rlc
	jmp 1b

2:	mov m,a

	lhld .retadr
	pchl
