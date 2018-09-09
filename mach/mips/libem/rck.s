#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Bounds check. Traps if the value is out of range.
 *  Stack: ( value descriptor -- value )
 *
 * This ".rck" only works with 4-byte integers.  The name is ".rck" and
 * not ".rck4" because many back ends only do rck with the word size.
 */

.sect .text
.define .rck
.rck:
	lw r4, 0(sp)            ! r4 = pointer to descriptor
	addiu sp, sp, 4         ! leave value on stack
	lw r5, 0(sp)            ! r5 = value

	lw at, 0(sp)            ! at = lower bound
	slt at, r5, at          ! at = r5 < at
	bne at, zero, .trap_erange

	lw at, 4(sp)            ! at = upper bound
	slt at, at, r5          ! at = at < r5
	bne at, zero, .trap_erange

	jr ra
	nop

.define .trap_erange
.trap_erange:
	li r4, 1
	j .trp
	nop
