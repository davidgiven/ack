.sect .text
.define _close
_close:
	addi r0, r0, 6		! close
	lwz r3, 0(sp)		! fd
	sc 0
	b .set_errno
	bclr 20, 0, 0
