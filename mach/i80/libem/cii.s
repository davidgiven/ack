.define .cii
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Convert integer to integer
! Expects in a-reg:	1 for signed integer to signed integer (cii)
!			0 for unsigned integer to unsigned integer (cuu)
! Expects on stack:	destination size
!			source size
!			source
! Yields on stack:	result

.cii:	pop h
	shld .retadr
	mov h,b
	mov l,c
	shld .bcreg

	sta .areg	! save a-register
	pop b
	mov e,c
	pop b		! c = source size
	mov b,e		! b = destination size
	mov a,b
	cmp c
	jz 3f		! destination size = source size
	jc shrink	! destination size < source size

! if destination size > source size only:
	lxi h,0
	dad sp
	mov e,l
	mov d,h		! de = stackpointer
	mov a,b
	sub c		! c = (still) source size
	mov b,a		! b = destination size - source size
	cma
	mov l,a
	mvi h,255
	inx h
	dad d		! hl = stackpointer - (dest. size - source size)
	sphl		! new stackpointer

1:	ldax d		! move source downwards
	mov m,a
	inx d
	inx h
	dcr c
	jnz 1b

	ral		! a-reg still contains most significant byte of source
	jnc 1f		! jump if is a positive integer
	lda .areg
	ora a
	jz 1f		! jump if it is a cuu
	mvi c,255	! c-reg contains filler byte

1:	mov m,c		! fill
	inx h
	dcr b
	jnz 1b
	jmp 3f		! done

!if destination size < source size only:
shrink:	mov l,b		! load destination size in hl
	mvi h,0
	dad sp
	mov d,h
	mov e,l		! de points just above lowest bytes of source
	mov l,c		! load source size in hl
	mvi h,0
	dad sp		! hl points just above "destination"

1:	dcx d		! move upwards
	dcx h
	ldax d
	mov m,a
	dcr b
	jnz 1b
	sphl

3:	lhld .bcreg
	mov b,h
	mov c,l
	lhld .retadr
	pchl
