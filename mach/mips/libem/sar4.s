#
.sect .text; .sect .rom; .sect .data; .sect .bss

/* Store to bounds-checked array.
 *
 * Stack: ( element array-adr index descriptor-adr -- )
 */

.sect .text
.define .sar4
.sar4:
	mov r25, ra

	jal .aar4
	nop

	/* pass r2 = size from .aar4 to .sts4

	jal .sts4
	nop

	jr r25
	nop

