.sect .text; .sect .rom; .sect .data; .sect .bss
.define _read
.sect .text
_read:
	mov	ax,3
	jmp	syscal
