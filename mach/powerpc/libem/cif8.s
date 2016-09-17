#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"

.sect .text

! Converts a 32-bit integer into a 64-bit double.
!
! Stack: ( int -- double )

.define .cif8
.cif8:
	addi sp, sp, -4          ! make space for the double
	
	lwz r3, 4(sp)
	xoris r3, r3, 0x8000
	stw r3, 4(sp)            ! flip sign of integer value
	
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
	.data4 0x80000000
