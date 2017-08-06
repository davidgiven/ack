.sect .text
.define .set_errno
.set_errno:
	li32 r10, _errno
	stw r3, 0(r10)		! set errno
	addi r3, r0, -1		! return -1
	bclr 20, 0, 0
