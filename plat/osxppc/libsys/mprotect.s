.sect .text
.define _mprotect
_mprotect:
	addi r0, r0, 74		! mprotect
	lwz r3, 0(sp)		! address
	lwz r4, 4(sp)		! length
	lwz r5, 8(sp)		! protection
	sc 0
	b .set_errno
	bclr 20, 0, 0
