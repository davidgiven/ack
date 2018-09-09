#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Load from bounds-checked array.
 *
 * Stack: ( array-adr index descriptor-adr -- element )
 */

.sect .text
.define .lar4
.lar4:
	mov r25, ra

	jal .aar4
	nop

	/* pass r2 = size from .aar4 to .los4

	jal .los4
	nop

	jr r25
	nop
