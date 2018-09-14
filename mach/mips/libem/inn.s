#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Tests a bit in a bitset on the stack.
 *
 * Stack: ( bitset bitnum setsize -- bool )
 *
 * Some back ends push false if bitnum is too large.  We don't because
 * the compilers tend to pass a small enough bitnum.
 */

.sect .text
.define .inn
.inn:
	lw r4, 0(sp)        ! r4 = size of set (bytes)
	lw r5, 4(sp)        ! r5 = bit number
	addiu sp, sp, 4     ! sp now points to word below bitset

	andi r6, r5, ~31    ! r6 = bit offset of base of word in set
	srl r6, r6, 3		! r6 = byte offset of base of word in set
	addu r6, sp, r6     ! r6 = address of word in set
	lw r6, 4(r6)        ! r6 = word (remember stack offset)

	andi r7, r5, 31     ! r7 = bit number within word
	srlv r6, r6, r7     ! r7 = candidate bit now at bit 0
	andi r6, r6, 1      ! r7 = bool

	addu sp, sp, r4     ! retract over bitfield (remember stack offset)
	sw r6, 0(sp)        ! store result

	jr ra
	nop
