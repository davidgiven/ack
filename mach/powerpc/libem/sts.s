.sect .text

! Stores a variable-sized structure from the stack.
!
!     r3 = size
!     r4 = address

.define .sts
.sts:
	! These sizes are handled specially.

	lwz r5, 0(sp)

	cmplwi r3, 1
	ble size1

	cmplwi r3, 2
	ble size2

	cmplwi r3, 4
	ble size4

	! Variable-sized structure.

	addi r3, r3, 3
	clrrwi r3, r3, 2         ! align size

	srwi r3, r3, 2           ! convert size to the number of words
	mtspr ctr, r3

1:
	lwz r5, 0(sp)
	addi sp, sp, 4
	stw r5, 0(r4)
	addi r4, r4, 4

	bdnz 1b                  ! decrement CTR, jump if non-zero
	blr

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
	blr
