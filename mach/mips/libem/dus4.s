#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Duplicates some words on top of stack.
 *  Stack: ( a size -- a a )
 */

.sect .text
.define .dus4
.dus4:
	lw r4, 0(sp)        ! r4 = size
	addiu sp, sp, 4     ! sp = pointer to a
	mov r5, sp          ! r5 = pointer to a
	subu sp, sp, r4     ! sp = pointer to newa
	mov r6, sp          ! r6 = pointer to b

	srl r4, r4, 2       ! r4 = number of words
1:
	lw at, 0(r5)
	sw at, 0(r6)
	addiu r5, r5, 4
	addiu r6, r6, 4
	addiu r4, r4, -1
	bne r4, zero, 1b
	nop

	jr ra
	nop
