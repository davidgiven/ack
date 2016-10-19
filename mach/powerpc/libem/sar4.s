#
#include "powerpc.h"

.sect .text

! Store to bounds-checked array.
!
! On entry:
!    r3 = ptr to descriptor
!    r4 = index
!    r5 = address of array

.define .sar4
.sar4:
	mfspr r10, lr
	bl .aar4
	mtspr lr, r10
	! r3 = ptr to element
	! r0 = size of element

	cmpi cr0, 0, r0, 1
	bc IFFALSE, EQ, 1f
	! Store 1 byte.
	lwz r4, 0(sp)
	addi sp, sp, 4
	stb r4, 0(r3)
	bclr ALWAYS, 0, 0
1:
	cmpi cr0, 0, r0, 2
	bc IFFALSE, EQ, 2f
	! Store 2 bytes.
	lwz r4, 0(sp)
	addi sp, sp, 4
	sth r4, 0(r3)
	bclr ALWAYS, 0, 0
2:
	! Store r0 bytes, where r0 must be a positive multiple of 4.
	or r5, r0, r0		! index r5 = length r0
3:
	addic. r5, r5, -4	! r5 -= 4
	lwzx r4, r5, sp
	stwx r4, r5, r3
	bc IFTRUE, GT, 3b	! loop if r5 > 0
	add sp, r0, sp		! move stack pointer up
	bclr ALWAYS, 0, 0
