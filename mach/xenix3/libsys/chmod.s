.sect .text; .sect .rom; .sect .data; .sect .bss
.define _chmod
.sect .text
_chmod:
	mov	ax,15
	jmp	sysal
