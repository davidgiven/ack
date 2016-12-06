.sect .text
.define _getdirentries
_getdirentries:
	addi r0, r0, 196	! getdirentries
	lwz r3, 0(sp)		! fd
	lwz r4, 4(sp)		! buffer
	lwz r5, 8(sp)		! buffer size
	lwz r6, 12(sp)		! base pointer
	sc 0
	b .set_errno
	bclr 20, 0, 0
