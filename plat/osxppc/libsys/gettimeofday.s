.sect .text
.define _gettimeofday
_gettimeofday:
	addi r0, r0, 116	! gettimeofday
	lwz r3, 0(sp)		! timeval pointer
	lwz r4, 4(sp)		! timezone pointer
	sc 0
	b .set_errno
	bclr 20, 0, 0
