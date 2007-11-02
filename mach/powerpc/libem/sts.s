#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"
	
.sect .text

! Stores a variable-sized structure from the stack.
!
!     r3 = size
!     r4 = address

.define .sts
.sts:
	! These sizes are handled specially.
	
	lwz r5, 0(sp)

	cmpi cr0, 0, r3, 1
	bc IFFALSE, GT, size1
	
	cmpi cr0, 0, r3, 2
	bc IFFALSE, GT, size2
	
	cmpi cr0, 0, r3, 4
	bc IFFALSE, GT, size4
	
	! Variable-sized structure.
	
	addi r3, r3, 3
	andi. r3, r3, ~3         ! align size
	
	srawi r3, r3, 2          ! convert size to the number of words
	mtspr ctr, r3
	
1:
	lwz r5, 0(sp)
	addi sp, sp, 4
	stw r5, 0(r4)
	addi r4, r4, 4

	bc DNZ, 0, 1b            ! decrement CTR, jump if non-zero
	bclr ALWAYS, 0, 0
	
size1:
	stb r5, 0(r4)
	b 1f
size2:
	sth r5, 0(r4)
	b 1f
size4:
	stw r5, 0(r4)
1:
	addi sp, sp, 4
	bclr ALWAYS, 0, 0
