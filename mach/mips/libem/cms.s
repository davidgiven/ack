#
.sect .text; .sect .rom; .sect .data; .sect .bss

/*
 * Set comparison Returns 0 if the sets were equal.
 * Stack: ( a b size -- a!=b )
 */

.sect .text
.define .cms
.cms:
	lw r4, 0(sp)        ! r4 = size; sp points at b-word
	addu r5, sp, r4     ! r5 points at a-word
	addu r6, r5, r4		! r6 is final sp-word
	srl r4, r4, 2       ! r4 = count of words
	li r8, 1			! result

1:
	lw at, 4(r5)        ! load a
	lw r7, 4(sp)        ! load b
	bne at, r7, exit	! branch if not equal
	nop
	addiu r5, r5, 4
	addiu sp, sp, 4
	addiu r4, r4, -1
	bne r4, zero, 1b
	nop

	li r8, 0			! Update result.
exit:
	mov sp, r6
	sw r8, 0(sp)
	jr ra
	nop

