.sect .text; .sect .rom; .sect .data; .sect .bss
.define _ulimit
.sect .text
_ulimit:
	mov	ax,63
	jmp	syscal
