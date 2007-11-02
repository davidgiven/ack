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
! On entry: r3 = address of CSA table
!           r4 = value

.define .csa
.csa:
	lwz r5, 0(r3)            ! load default
	mtspr ctr, r5
	
	lwz r5, 4(r3)            ! fetch lower bound
	subf. r4, r5, r4         ! adjust value
	bcctr IFTRUE, LT, 0      ! jump to default if out of range
	
	lwz r5, 8(r3)            ! fetch range
	cmp cr0, 0, r4, r5
	bcctr IFTRUE, GT, 0      ! jump to default if out of range
	
	addi r3, r3, 12          ! skip header
	rlwinm r4, r4, 2, 0, 31-2 ! scale value (<<2)
	b 1f
1:
	lwzx r5, r3, r4          ! load target
	b 1f
1:
	mtspr ctr, r5
	 
	or. r5, r5, r5           ! test it
	b 1f
1:
	bcctr IFFALSE, EQ, 0     ! jump to target if non-zero
	b .trap_ecase            ! otherwise trap
