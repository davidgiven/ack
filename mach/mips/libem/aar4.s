#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Get address of element of bounds-checked array.
 *
 * Stack: ( array-adr index descriptor-adr -- element-adr )
 * Sets r2 = size of element for .los4, .sts4
 * Preserves r25 (the last volatile register)
 */

.sect .text
.define .aar4
.aar4:
	lw r4, 0(sp)            ! r4 = address of descriptor
	lw r5, 0(sp)            ! r5 = index
	lw r6, 0(sp)            ! r6 = address of array

	lw r7, 0(r4)            ! at = lower bound
	slt at, r7, at          ! at = r5 < at
	bne at, zero, .trap_earray

	lw at, 4(r4)            ! at = upper bound
	slt at, at, r5          ! at = at < r5
	bne at, zero, .trap_earray

	lw r2, 8(r4)            ! r2 = size of element
	subu r5, r5, r7         ! adjust index for non-zero lower bound
	mul r5, r5, r2          ! scale index by size
	addu r5, r5, r6         ! calculate address of element

	sw r5, 8(sp)            ! push address of element
	addiu sp, sp, 8
	jr ra
	nop

.define .trap_earray
.trap_earray:
	li r4, 0                 ! EARRAY = 0 in h/em_abs.h
	b .trp
