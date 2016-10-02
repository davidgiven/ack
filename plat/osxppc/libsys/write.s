.sect .text
.define _write
_write:
	addi r0, r0, 4		! write
	lwz r3, 0(sp)		! fd
	lwz r4, 4(sp)		! buffer
	lwz r5, 8(sp)		! buffer size
	sc 0
	b .set_errno
	bclr 20, 0, 0
