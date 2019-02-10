#
.sect .text; .sect .rom; .sect .data; .sect .bss

/*
 * Create empty set.
 * Stack: ( size -- set )
 */

.sect .text
.define .zer
.zer:
	lw r4, 0(sp)        ! r4 = size
	addiu sp, sp, 4
	srl r4, r4, 2       ! r4 = word count

1:
	addiu sp, sp, -4
	sw zero, 0(sp)
	addiu r4, r4, -1
	bne r4, zero, 1b
	nop

	jr ra
	nop

