#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"
	
.sect .text

! Loads a variable-sized structure onto the stack.
!
!     r3 = size
!     r4 = address

.define .los
.los:
	! These sizes are handled specially.
	
	cmpi cr0, 0, r3, 1
	bc IFFALSE, GT, size1
	
	cmpi cr0, 0, r3, 2
	bc IFFALSE, GT, size2
	
	cmpi cr0, 0, r3, 4
	bc IFFALSE, GT, size4
	
	! Variable-sized structure.
	
	addi r3, r3, 3
	andi. r3, r3, ~3         ! align size
	
	add r4, r4, r3           ! adjust address to top of block

	srawi r3, r3, 2          ! convert size to the number of words
	mtspr ctr, r3
	
1:
	lwzu r5, -4(r4)
	stwu r5, -4(sp)
	bc DNZ, 0, 1b            ! decrement CTR, jump if non-zero
	bclr ALWAYS, 0, 0
	
size1:
	lbz r3, 0(r4)
	b 1f
size2:
	lhz r3, 0(r4)
	b 1f
size4:
	lwz r3, 0(r4)
1:
	stwu r3, -4(sp)
	bclr ALWAYS, 0, 0
