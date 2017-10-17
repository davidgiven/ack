.sect .text; .sect .rom; .sect .data; .sect .bss

.sect .text

! Converts a 32-bit integer into a 64-bit double.
!
! Stack: ( int -- double )

.define .cif8
.cif8:
	! Conversion uses the pivot value
	!   1 << 52 = 0x4330 0000 0000 0000
	!
	! From signed integer i, we compute
	!   ((1 << 52) + (1 << 31) + i) - ((1 << 52) + (1 << 31))

	lis r3, 0x4330
	stwu r3, -4(sp)          ! make space for the double

	lwz r3, 4(sp)
	xoris r3, r3, 0x8000
	stw r3, 4(sp)            ! flip sign bit to get (1 << 31) + i

	lfd f0, 0(sp)            ! f0 = (1 << 52) + (1 << 31) + i
	lis r3, 0x8000
	stw r3, 4(sp)
	lfd f1, 0(sp)            ! f1 = (1 << 52) + (1 << 31)
	fsub f0, f0, f1          ! finish conversion

	stfd f0, 0(sp)           ! save value again...
	blr                      ! ...and return 
