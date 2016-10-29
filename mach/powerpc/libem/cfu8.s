#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"

.sect .text

! Converts a 64-bit double into a 32-bit unsigned integer.
!
! Stack: ( double -- uint )

.define .cfu8
.cfu8:
	li32 r3, .fd_00000000
	lfd f0, 0(r3)            ! f0 = 0.0
	
	lfd f1, 0(sp)            ! value to be converted

	li32 r3, .fd_FFFFFFFF
	lfd f3, 0(r3)            ! f3 = 0xFFFFFFFF	

	li32 r3, .fd_80000000
	lfd f4, 0(r3)            ! f4 = 0x80000000 	
	
	fsel f2, f1, f1, f0
	fsub f5, f3, f1
	fsel f2, f5, f2, f3
	fsub f5, f2, f4
	fcmpu cr0, f2, f4
	fsel f2, f5, f5, f2
	fctiwz f2, f2
	
	stfd f2, 0(sp)
	addi sp, sp, 4
	
	bclr IFTRUE, LT, 0
	
	lwz r3, 0(sp)
	xoris r3, r3, 0x8000
	stw r3, 0(sp)

	bclr ALWAYS, 0, 0 
