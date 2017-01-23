.sect .text

! Loads a variable-sized structure onto the stack.
!
!     r3 = size
!     r4 = address

.define .los
.los:
	! These sizes are handled specially.

	cmplwi r3, 1
	ble size1

	cmplwi r3, 2
	ble size2

	cmplwi r3, 4
	ble size4

	! Variable-sized structure.

	addi r3, r3, 3
	clrrwi r3, r3, 2         ! align size

	add r4, r4, r3           ! adjust address to top of block

	srwi r3, r3, 2           ! convert size to the number of words
	mtspr ctr, r3

1:
	lwzu r5, -4(r4)
	stwu r5, -4(sp)
	bdnz 1b                  ! decrement CTR, jump if non-zero
	blr

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
	blr
