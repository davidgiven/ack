.sect .text; .sect .rom; .sect .data; .sect .bss
.define _brk
.sect .text
_brk:
	mov	ax,17
	jmp	sysal
