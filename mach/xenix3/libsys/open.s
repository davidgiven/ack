.sect .text; .sect .rom; .sect .data; .sect .bss
.define _open
.sect .text
_open:
	mov	ax,5
	jmp	sysal
