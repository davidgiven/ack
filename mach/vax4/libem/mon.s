.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.mon

	! $Id$


.mon:
	pushl	$EBADMON
	jmp	.fat
