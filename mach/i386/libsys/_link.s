.sect .text; .sect .rom; .sect .data; .sect .bss
.define __link
.sect .text
__link:
	mov	ax,9
	jmp	sysx
