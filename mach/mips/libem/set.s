#
.sect .text; .sect .rom; .sect .data; .sect .bss

/*
 * Create singleton set.
 * Stack: ( bitnumber size -- set )
 */

.sect .text
.define .set
.set:
	lw r4, 0(sp)        ! r4 = size
	lw r5, 4(sp)        ! r5 = bit number
	addiu sp, sp, 8
	srl r4, r4, 2       ! r4 = word count

	! Create an empty set by pushing zeros.

1:
	addiu sp, sp, -4
	sw zero, 0(sp)
	addiu r4, r4, -1
	bne r4, zero, 1b
	nop

	! sp now points at the set.

	andi r6, r5, ~31	! r6 = bit offset of base of word in set
	srl r6,r6, 3        ! r6 = byte offset of word in set
	addu r6, sp, r6		! r6 = address of word in set

	andi r7, r5, 31		! r7 = bit number within word
	li r8, 1
	sllv r8, r8, r7     ! r8 = word with 1 set
	sw r8, 0(r6)        ! write to set

	jr ra
	nop

