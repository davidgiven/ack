.sect .text; .sect .rom; .sect .data; .sect .bss
.define _kill
.sect .text
_kill:
	mov	ax,37
	jmp	sysx
