.sect .text
.define _getpid
_getpid:
	addi r0, r0, 20		! getpid
	sc 0
	b .set_errno
	bclr 20, 0, 0
