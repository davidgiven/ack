.sect .text
.define _read
_read:
	addi r0, r0, 3		! read
	lwz r3, 0(sp)		! fd
	lwz r4, 4(sp)		! buffer
	lwz r5, 8(sp)		! buffer size
	sc 0
	b .set_errno
	bclr 20, 0, 0
