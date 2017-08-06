.sect .text
.define _fstat
_fstat:
	addi r0, r0, 189	! fstat
	lwz r3, 0(sp)		! fd
	lwz r4, 4(sp)		! stat pointer
	sc 0
	b .set_errno
	bclr 20, 0, 0
