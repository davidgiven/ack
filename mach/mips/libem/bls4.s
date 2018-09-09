#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Does a block move of words between non-overlapping buffers.
 * Stack: ( src dst len -- )
 */

.sect .text
.define .bls4
.bls4:
	lw r4, 0(sp)	! r4=len
	lw r5, 4(sp)	! r5=dst
	lw r6, 8(sp)	! r6=src
	addiu sp, sp, 12

	srl r4, r4, 2   ! convert len to words
1:
	beq r4, zero, 2f
	nop

	lw at, 0(r6)
	sw at, 0(r5)
	addiu r6, r6, 4
	addiu r5, r5, 4
	addiu r4, r4, -1
	b 1b

2:
	jr ra
	nop
