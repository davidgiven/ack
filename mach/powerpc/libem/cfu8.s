.sect .text

! Converts a 64-bit double into a 32-bit unsigned integer.
!
! Stack: ( double -- uint )

.define .cfu8
.cfu8:
	lis r3, ha16[.fd_00000000]
	lfd f0, lo16[.fd_00000000](r3) ! f0 = 0.0

	lfd f1, 0(sp)            ! value to be converted

	lis r3, ha16[.fd_FFFFFFFF]
	lfd f3, lo16[.fd_FFFFFFFF](r3) ! f3 = 0xFFFFFFFF

	lis r3, ha16[.fd_80000000]
	lfd f4, lo16[.fd_80000000](r3) ! f4 = 0x80000000

	fsel f2, f1, f1, f0
	fsub f5, f3, f1
	fsel f2, f5, f2, f3
	fsub f5, f2, f4
	fcmpu cr0, f2, f4
	fsel f2, f5, f5, f2
	fctiwz f2, f2
	
	stfd f2, 0(sp)
	addi sp, sp, 4

	bltlr

	lwz r3, 0(sp)
	xoris r3, r3, 0x8000
	stw r3, 0(sp)

	blr
