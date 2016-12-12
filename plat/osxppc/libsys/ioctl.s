.sect .text
.define _ioctl
_ioctl:
	addi r0, r0, 54		! ioctl
	lwz r3, 0(sp)		! fd
	lwz r4, 4(sp)		! command
	lwz r5, 8(sp)		! argument pointer
	sc 0
	b .set_errno
	bclr 20, 0, 0
