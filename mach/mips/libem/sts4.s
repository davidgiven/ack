#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Stores a variable-sized block from the stack.
 *
 * On entry: r2 = size
 * Stack: ( block address -- )
 * Preserves r25 for .lar4 and .sar4
 */

.sect .text
.define .sts4
.sts4:
	lw r4, 0(sp)            ! r4 = address
	addiu sp, sp, 4         ! sp = pointer to block

	! Sizes 1 and 2 are handled specially.

	li at, 1
	beq r2, at, byte_sized
	nop

	li at, 2
	beq r2, at, word_sized
	nop

	! Else the size must be a multiple of 4.

	srl r5, r2, 2           ! r5 = number of words

	! Copy.

1:
	lw at, 0(sp)
	sw at, 0(r4)
	addiu sp, sp, 4
	addiu r4, r4, 4
	addiu r5, r5, -1
	bne r5, zero, 1b
	nop

	jr ra
	nop

byte_sized:
	lw at, 0(sp)
	sb at, 0(r4)
	addiu sp, sp, 4
	jr ra
	nop

word_sized:
	lw at, 0(sp)
	sh at, 0(r4)
	addiu sp, sp, 4
	jr ra
	nop
