#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* this is not a subroutine, but just a
 * piece of code that computes the jump-
 * address and jumps to it.
 * traps if resulting address is zero
 *
 * Stack: ( value tableaddr -- )
 */

.sect .text
.define .csb
.csb:
	lw r4, 0(sp)             ! r4 = address of table
	lw r5, 0(sp)             ! r5 = value
	addiu sp, sp, 8

	lw r6, 0(r4)             ! r6 = default target
	lw r7, 4(r4)             ! r7 = table count

2:
	addiu r7, r7, -1         ! decrement count
	bltz r7, 1f              ! exit loop if out
	nop

	lw r8, 8(r4)             ! candidate value
	beq r8, r5, 3f           ! branch if equal
	nop

	addiu r4, r4, 8          ! next entry
	b 2b
	nop

3:
	/* We found an item in the table. */
	lw r6, 12(r4)            ! load jump target
	/* fall through */
1:
	beq r6, zero, 4f        /* trap if null */
	nop
	jr r6                   /* otherwise jump */
	nop
4:
	j .trap_ecase
	nop

