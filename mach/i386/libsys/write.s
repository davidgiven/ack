.sect .text; .sect .rom; .sect .data; .sect .bss
.define _write
.sect .text
_write:
	mov	ax,4
	jmp	syscal
