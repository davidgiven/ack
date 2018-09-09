#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Exchange top two values on stack.
 *   Stack: ( a b size -- b a )
 */

.sect .text
.define .exg
.exg:
	lw r4, 0(sp)            ! r4 = size
	srl r5, r4, 2           ! r5 = number of words
	addiu sp, sp, 4         ! adjust stack for input/output parameter size

	mov r6, sp              ! r6 = pointer to b
	addu r7, r6, r4         ! r7 = pointer to a

	! Loop to swap each pair of words.
1:
	lw r8, 0(r6)
	lw r9, 0(r7)
	sw r9, 0(r6)
	sw r8, 0(r7)
	addiu r6, r6, 4
	addiu r7, r7, 4
	addiu r5, r5, -1
	bne r5, zero, 1b
	nop

	jr ra
	nop
