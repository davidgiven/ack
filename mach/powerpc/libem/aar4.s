#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"

.sect .text
	
! Index into a bounds-checked array.
!
! On entry:
!    r3 = ptr to descriptor
!    r4 = index
!    r5 = address of array

.define .aar4
.aar4:
	la r0, .trap_earray
	mtspr ctr, r0            ! load CTR with trap address

	lwz r0, 0(r3)
	subf. r4, r0, r4         ! adjust range
	bcctr IFTRUE, LT, 0      ! check lower bound
	
	lwz r0, 4(r3)
	cmpl cr0, 0, r4, r3
	bcctr IFFALSE, LT, 0     ! check upper bound
	
	lwz r0, 8(r3)
	mullw r4, r4, r0         ! scale index
	add r3, r4, r5           ! calculate element address
	
	bclr ALWAYS, 0, 0
