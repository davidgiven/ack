.sect .text
.define _lseek
_lseek:
	addi r0, r0, 199	! lseek
	lwz r3, 0(sp)		! fd
	! ack passes 4-byte off_t, but system call takes 8-byte off_t
	addi r4, r0, 0		! offset (high word)
	lwz r5, 4(sp)		! offset (low word)
	lwz r6, 8(sp)		! whence
	sc 0
	b .set_errno
	or r3, r4, r4		! return offset (low word)
	bclr 20, 0, 0
