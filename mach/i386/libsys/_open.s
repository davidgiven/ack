.sect .text; .sect .rom; .sect .data; .sect .bss
.define __open
.sect .text
__open:
	mov	ax,5
	jmp	syscal
