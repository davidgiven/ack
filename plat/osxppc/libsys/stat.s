.sect .text
.define _stat
_stat:
	addi r0, r0, 188	! stat
	lwz r3, 0(sp)		! path
	lwz r4, 4(sp)		! stat pointer
	sc 0
	b .set_errno
	bclr 20, 0, 0
