.define .com

! Complement bytes on top of stack.
! Expects in de-registers: number of bytes

.com:	pop h
	shld .retadr
	lxi h,0
	dad sp
1:	mov a,m
	cma
	mov m,a
	inx h
	dcx d
	mov a,e
	ora d
	jnz 1b
	lhld .retadr
	pchl

