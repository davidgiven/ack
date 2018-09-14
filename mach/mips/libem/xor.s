#
.sect .text; .sect .rom; .sect .data; .sect .bss

/*
 * Set xor..
 * Stack: ( a b size -- a^b )
 */

.sect .text
.define .xor
.xor:
	lw r4, 0(sp)        ! r4 = size
	addiu sp, sp, 4     ! sp points at b
	addu r5, sp, r4     ! r5 points at a
	srl r4, r4, 2       ! r4 = count of words

1:
	lw at, 0(r5)        ! load a
	lw r6, 0(sp)        ! load b
	xor at, at, r6      ! combine
	sw at, 0(r5)        ! write back to a
	addiu r5, r5, 4
	addiu sp, sp, 4
	addiu r4, r4, -1
	bne r4, zero, 1b
	nop

	jr ra
	nop

