#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"
	
.sect .text

! Converts a 32-bit unsigned integer into a 64-bit double.
!
! Stack: ( uint -- double )

.define .cuf8
.cuf8:
	addi sp, sp, -4          ! make space for the double
	
	addis r3, r0, 0x4330
	stw r3, 0(sp)            ! set high word to construct a double
	
	lfd f0, 0(sp)            ! load value
	
	la r3, pivot
	lfd f1, 0(r3)            ! load pivot value
	fsub f0, f0, f1          ! adjust
	                         
	stfd f0, 0(sp)           ! save value again...
	bclr ALWAYS, 0, 0        ! ...and return 

.sect .rom
pivot:
	.data4 0x43300000
	.data4 0x00000000
