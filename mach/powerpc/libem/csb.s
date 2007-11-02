#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"
	
.sect .text

! this is not a subroutine, but just a
! piece of code that computes the jump-
! address and jumps to it.
! traps if resulting address is zero
!
! On entry: r3 = address of CSB table
!           r4 = value

.define .csb
.csb:
	lwz r5, 0(r3)            ! load default
	mtspr ctr, r5
	
	lwz r6, 4(r3)            ! fetch count
	
1:
	or. r6, r6, r6           ! test count
	bcctr IFTRUE, EQ, 0      ! exit if zero
	addi r6, r6, -1          ! otherwise decrement
	
	lwzu r7, 8(r3)           ! fetch target index, increment pointer
	cmp cr0, 0, r4, r7       ! compare with value
	bc IFFALSE, EQ, 1b       ! if not equal, go again
	
	lwz r7, 4(r3)            ! fetch target address
	mtspr ctr, r7
	
	or. r7, r7, r7           ! test it
	bcctr IFFALSE, EQ, 0     ! jump to target if non-zero
	b .trap_ecase            ! otherwise trap
