.sect .text; .sect .rom; .sect .data; .sect .bss
.define _getuid
.sect .text
_getuid:
	mov	ax,24
	jmp	syscal
