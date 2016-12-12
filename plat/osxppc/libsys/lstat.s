.sect .text
.define _lstat
_lstat:
	addi r0, r0, 190	! lstat
	lwz r3, 0(sp)		! path
	lwz r4, 4(sp)		! stat pointer
	sc 0
	b .set_errno
	bclr 20, 0, 0
