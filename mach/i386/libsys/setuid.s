.sect .text; .sect .rom; .sect .data; .sect .bss
.define _setuid
.sect .text
_setuid:
	mov	ax,23
	jmp	sysx
