.define .sri2

! Shift 16 bits signed or unsigned integer right
! Expects in a-reg.:	1 if signed integer
!			0 if unsigned integer
! Expects on stack:	number of shifts
!			number to be shifted
! Yields in de-reg.:	result

.sri2:	pop h
	shld .retadr

	pop h		!hl = number of shifts
	pop d		!de = number to be shifted
	mvi h,0
	ora a
	jz 1f		!jump if unsigned integer
	mov a,d
	ral
	jnc 1f		!jump if positive signed integer
	mvi h,255	!now h=1 if negative signed number, h=0 otherwise.

1:	mov a,l		!return 0 or -1 if hl>=16
	cpi 16
	jnc 3f

2:	dcr l
	jm 4f
	mov a,h
	rar		!set carry bit correct
	mov a,d
	rar
	mov d,a
	mov a,e
	rar
	mov e,a
	jmp 2b

3:	mov d,h
	mov e,h

4:	lhld .retadr
	pchl
