#
.sect .text; .sect .rom; .sect .data; .sect .bss

/*
 * Set complement.
 * Stack: ( a size -- ~a )
 */

.sect .text
.define .com
.com:
	lw r4, 0(sp)        ! r4 = size
	addiu sp, sp, 4
	mov r5, sp          ! r5 points to set
	srl r4, r4, 2       ! r4 = word count

1:
	lw at, 0(r5)
	nor at, zero, at
	sw at, 0(r5)
	addiu r5, r5, 4
	addiu r4, r4, -1
	bne r4, zero, 1b
	nop

	jr ra
	nop

