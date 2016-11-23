! The system call checks the timeval pointer but doesn't store the
! time there.  If the pointer wasn't NULL, then the system call
! returns the time in a pair of registers.

.sect .text
.define _gettimeofday
_gettimeofday:
	addi r0, r0, 116	! gettimeofday
	lwz r3, 0(sp)		! timeval pointer
	lwz r4, 4(sp)		! timezone pointer
	or. r5, r3, r3
	sc 0
	b .set_errno
	bc 12, 2, 1f		! beq 1f
	stw r3, 0(r5)		! seconds
	stw r4, 4(r5)		! microseconds
1:
	addi r3, r0, 0		! return 0
	bclr 20, 0, 0
