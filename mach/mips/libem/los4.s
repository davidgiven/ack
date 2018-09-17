#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Loads a variable-sized block onto the stack.
 *
 * On entry: r2 = size
 * Stack: ( address -- block )
 * Preserves r25 for .lar4 and .sar4
 */

.sect .text
.define .los4
.los4:
	lw r4, 0(sp)            ! r4 = address of source block

	! Sizes 1 and 2 are handled specially.

	li at, 1
	beq r2, at, byte_sized
	nop

	li at, 2
	beq r2, at, word_sized
	nop

	! Else the size must be a multiple of 4.

	srl r5, r2, 2           ! r5 = number of words
	addiu sp, sp, 4         ! retract over address
	subu sp, sp, r2         ! allocate space for destination block
	mov r6, sp              ! r6 = start of destination block

1:
	lw at, 0(r4)
	sw at, 0(r6)
	addiu r4, r4, 4
	addiu r6, r6, 4
	addiu r5, r5, -1
	bne r5, zero, 1b
	nop

	jr ra
	nop

byte_sized:
	lbu at, 0(r4)
	sw at, 0(sp)
	jr ra
	nop

word_sized:
	lhu at, 0(r4)
	sw at, 0(sp)
	jr ra
	nop
