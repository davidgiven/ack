.sect .text; .sect .rom; .sect .data; .sect .bss
.define _link
.sect .text
_link:
	mov	ax,9
	jmp	sysx
