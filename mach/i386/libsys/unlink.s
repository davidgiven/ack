.sect .text; .sect .rom; .sect .data; .sect .bss
.define _unlink
.sect .text
_unlink:
	mov	ax,10
	jmp	sysx
