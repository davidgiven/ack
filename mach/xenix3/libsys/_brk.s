.sect .text; .sect .rom; .sect .data; .sect .bss
.define __brk
.sect .text
__brk:
	mov	ax,17
	jmp	sysal
