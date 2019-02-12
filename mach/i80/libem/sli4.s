.define .sli4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Shift 32 bits integer left
! Expects on stack:	number of shifts
!			number to be shifted
! Yields on stack:	result

.sli4:
	pop h
	shld .retadr

	pop b		!number of shifts
	pop d		!low-order bytes of number to be shifted
	pop h		!high-order bytes
	mov a,b		!if bc>=32 return 0
	ora a
	jnz 2f
	mov a,c
	cpi 32
	jnc 2f
1:	dcr c
	jm 3f
	dad h
	xchg
	dad h
	xchg
	jnc 1b
	inx h
	jmp 1b

2:	lxi h,0
	lxi d,0

3:	push h
	push d

	lhld .retadr
	pchl

