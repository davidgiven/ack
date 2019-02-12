.define .sri4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Shift 32 bits signed or unsigned integer right
! Expects in a-reg.:	1 if signed integer
!			0 if unsigned integer
! Expects on stack:	number of shifts
!			number to be shifted
! Yields on stack:	result

.sri4:	pop h
	shld .retadr

	pop b		!number of shifts
	pop d		!low-order bytes of number to be shifted
	pop h		!high-order bytes
	mvi b,0
	ora a
	jz 1f		!jump if unsigned integer
	mov a,h
	ral
	jnc 1f		!jump if positive signed integer
	mvi b,255

1:	mov a,c
	cpi 32
	jnc 3f

2:	dcr c
	jm 4f
	mov a,b
	rar
	mov a,h
	rar
	mov h,a
	mov a,l
	rar
	mov l,a
	mov a,d
	rar
	mov d,a
	mov a,e
	rar
	mov e,a
	jmp 2b

3:	mov d,b
	mov e,b
	mov h,b
	mov l,b

4:	push h
	push d

	lhld .retadr
	pchl
