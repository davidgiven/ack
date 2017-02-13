.sect .text

! Stores a variable-sized block from the stack.
!
! On entry: r3 = size
! Stack: ( block address -- )
! Preserves r10 for .sar4

.define .sts4
.sts4:
	lwz r4, 0(sp)            ! r4 = address

	! Sizes 1 and 2 are handled specially.
	cmplwi r3, 1
	ble 1f
	cmplwi r3, 2
	ble 2f

	! Else the size must be a multiple of 4.
	srwi r5, r3, 2
	mtspr ctr, r5            ! ctr = number of words
	addi r4, r4, -4          ! adjust address to before block
4:	lwzu r5, 4(sp)
	stwu r5, 4(r4)
	bdnz 4b                  ! decrement ctr, jump if non-zero
	addi sp, sp, 4
	blr

1:	lwz r5, 4(sp)
	stb r5, 0(r4)
	b 3f
2:	lwz r5, 4(sp)
	sth r5, 0(r4)
3:	addi sp, sp, 8
	blr
