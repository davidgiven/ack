.sect .text; .sect .rom; .sect .data; .sect .bss
.define _nice
.sect .text
_nice:
	mov	ax,34
	jmp	syscal
