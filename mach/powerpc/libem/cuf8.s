.sect .text

! Converts a 32-bit unsigned integer into a 64-bit double.
!
! Stack: ( uint -- double )

.define .cuf8
.cuf8:
	! Conversion uses the pivot value
	!   1 << 52 = 0x4330 0000 0000 0000
	!
	! From unsigned integer u, we compute
	!   ((1 << 52) + u) - (1 << 52)

	lis r3, 0x4330
	stwu r3, -4(sp)          ! make space for the double

	lfd f0, 0(sp)            ! f0 = (1 << 52) + u
	li r3, 0x0000
	stw r3, 4(sp)
	lfd f1, 0(sp)            ! f1 = (1 << 52)
	fsub f0, f0, f1          ! finish conversion

	stfd f0, 0(sp)           ! save value again...
	blr                      ! ...and return
