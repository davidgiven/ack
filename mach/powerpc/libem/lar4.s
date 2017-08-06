.sect .text

! Load from bounds-checked array.
!
! On entry:
!    r3 = ptr to descriptor
!    r4 = index
!    r5 = address of array

.define .lar4
.lar4:
	mfspr r10, lr
	bl .aar4
	mtspr lr, r10
	! r3 = ptr to element
	! r0 = size of element

	cmpwi r0, 1
	bne 1f
	! Load 1 byte.
	lbz r4, 0(r3)
	stwu r4, -4(sp)
	blr
1:
	cmpwi r0, 2
	bne 2f
	! Load 2 bytes.
	lhz r4, 0(r3)
	stwu r4, -4(sp)
	blr
2:
	! Load r0 bytes, where r0 must be a positive multiple of 4.
	subf sp, r0, sp		! move stack pointer down
	or r5, r0, r0		! index r5 = length r0
3:
	addic. r5, r5, -4	! r5 -= 4
	lwzx r4, r5, r3
	stwx r4, r5, sp
	bgt 3b			! loop if r5 > 0
	blr
