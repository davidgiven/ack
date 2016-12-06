.sect .text
.define _open
_open:
	addi r0, r0, 5		! open
	lwz r3, 0(sp)		! path
	lwz r4, 4(sp)		! flags
	lwz r5, 8(sp)		! mode
	sc 0
	b .set_errno
	bclr 20, 0, 0
