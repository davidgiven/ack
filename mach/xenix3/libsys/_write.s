.sect .text; .sect .rom; .sect .data; .sect .bss
.define __write
.sect .text
__write:
	mov	ax,4
	jmp	sysal
