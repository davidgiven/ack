.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.mon

	! $Header$


.mon:
	pushl	$EBADMON
	jmp	.fat
