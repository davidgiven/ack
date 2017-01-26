.sect .text

! Converts a 64-bit double into a 32-bit integer.
!
! Stack: ( double -- int )

.define .cfi8
.cfi8:
	lfd f0, 0(sp)
	fctiwz f0, f0
	stfd f0, 0(sp)
	addi sp, sp, 4
	blr
