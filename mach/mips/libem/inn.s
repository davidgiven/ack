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
	lw r5, 0(sp)        ! r5 = bit number
	addiu sp, sp, 8     ! sp now points at bitset

	srl r6, r5, 3       ! r6 = offset of word in set
	addu r6, sp, r6     ! r6 = address of word in set
	lw r6, 0(r6)        ! r6 = word

	ext r7, r5, 0, 3    ! r7 = bit number within word
	srlv r6, r6, r7     ! r7 = candidate bit now at bit 0
	andi r6, r6, 1      ! r7 = bool

	addu sp, sp, r4     ! retract over bitfield

	addiu sp, sp, -4
	sw r6, 0(sp)        ! push result

	jr ra
	nop
