.sect .text; .sect .rom; .sect .data; .sect .bss
.define __kill
.sect .text
__kill:
	mov	ax,37
	jmp	sysx
