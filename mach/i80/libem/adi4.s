.define .adi4

! Add two 32 bits signed or unsigned integers
! Expects on stack: operands
! Yields on stack:  result

.adi4:	pop h
	shld .retadr	! get return address out of the way
	pop d
	pop h
	xthl
	dad d
	shld .tmp1
	pop d
	pop h
	jnc 1f
	inx h
1:	dad d
	push h
	lhld .tmp1
	push h
	lhld .retadr
	pchl
