.sect .text
.define _mmap
_mmap:
	addi r0, r0, 197	! mmap
	lwz r3, 0(sp)		! address
	lwz r4, 4(sp)		! length
	lwz r5, 8(sp)		! protection
	lwz r6, 12(sp)		! flags
	lwz r7, 16(sp)		! fd
	! ack passes 4-byte off_t, but system call takes 8-byte off_t
	addi r8, r0, 0		! offset (high word)
	lwz r9, 20(sp)		! offset (low word)
	sc 0
	b .set_errno
	bclr 20, 0, 0
