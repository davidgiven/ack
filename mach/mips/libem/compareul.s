#
.sect .text; .sect .rom; .sect .data; .sect .bss

/*
 * 64-big unsigned tristate comparison.
 * Inputs:
 *    r2r3: left
 *    r4r5: right
 * Outputs;
 *    r2: result
 */

.sect .text
.define .compareul
.compareul:
	/* Compare high words. */

	sltu at, r3, r5			! if r3<r5, then 1
	subu at, zero, at		! if r3<r5, then -1
	bne at, zero, exit
	nop

	sltu at, r5, r3			! if r3>r5, then 1
	bne at, zero, exit
	nop

	/* High words are equal --- compare low words. */

	sltu at, r2, r4			! if r2<r4, then 1
	subu at, zero, at		! if r2<r4, then -1
	bne at, zero, exit
	nop

	sltu at, r4, r2			! if r2>r4, then 1
exit:
	mov r2, at
	jr ra
	nop

