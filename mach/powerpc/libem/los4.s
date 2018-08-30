.sect .text

! Loads a variable-sized block onto the stack.
!
! On entry: r3 = size
! Stack: ( address -- block )
! Preserves r10 for .lar4

.define .los4
.los4:
	lwz r4, 0(sp)            ! r4 = address

	! Sizes 1 and 2 are handled specially.
	cmplwi r3, 1
	ble 1f
	cmplwi r3, 2
	ble 2f

	! Else the size must be a multiple of 4.
	srwi r5, r3, 2
	mtspr ctr, r5            ! ctr = number of words
	addi sp, sp, 4
	add r4, r4, r3           ! adjust address to end of block
4:	lwzu r5, -4(r4)
	stwu r5, -4(sp)
	bdnz 4b                  ! decrement ctr, jump if non-zero
	blr

1:	lbz r5, 0(r4)
	stw r5, 0(sp)
	blr
2:	lhz r5, 0(r4)
	stw r5, 0(sp)
	blr
