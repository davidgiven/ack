.sect .text; .sect .rom; .sect .data; .sect .bss
.define _getgid
.sect .text
_getgid:
	mov	ax,47
	jmp	syscal
