.sect .text
.define _kill
_kill:
	addi r0, r0, 37		! kill
	lwz r3, 0(sp)		! pid
	lwz r4, 4(sp)		! signal
	sc 0
	b .set_errno
	bclr 20, 0, 0
