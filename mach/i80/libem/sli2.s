.define .sli2

! Shift 16 bits integer left
! Expects on stack:	number of shifts
!			number to be shifted
! Yields in de-reg:	result

.sli2:	pop h
	shld .retadr

	pop d		!de = number of shifts
	pop h		!hl= number to be shifted
	mov a,d		!if de>15 return zero
	ora a
	jnz 2f
	mov a,e
	cpi 16
	jnc 2f
1:	dcr e
	jm 3f
	dad h
	jmp 1b

2:	lxi h,0
3:	xchg		!result in de-registers

	lhld .retadr
	pchl
