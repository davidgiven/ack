#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* This is not a subroutine, but just a
 * piece of code that computes the jump-
 * address and jumps to it.
 * traps if resulting address is zero
 *
 * Stack: ( value tableaddr -- )
 */

.define .csa
.csa:
	lw r4, 0(sp)            /* r4 = table */
	lw r5, 0(sp)            /* r5 = value */
	addiu sp, sp, 8

	lw r6, 0(r4)            /* r6 = default target */
	lw r7, 4(r4)            /* r7 = lower bound */
	subu r5, r5, r6         /* adjust value */
	bltz r5, 1f             /* jump to default if out of range */
	nop

	lw r7, 8(r4)            /* fetch range */
	subu r7, r5, r7         /* compute (adjusted value - range) */
	blez r7, 1f             /* jump to default if out of range */
	nop

	addiu r4, r4, 12        /* skip header */
	sll r5, r5, 2           /* value = value<<2 */
	addu r4, r4, r5         /* find address of target */
	lw r6, 0(r4)            /* r6 = new target */

1:
	beq r6, zero, 2f        /* trap if null */
	nop
	jr r6                   /* otherwise jump */
	nop
2:
	j .trap_ecase
	nop
