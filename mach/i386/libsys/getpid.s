.sect .text; .sect .rom; .sect .data; .sect .bss
.define _getpid
.sect .text
_getpid:
	mov	ax,20
	jmp	syscal
