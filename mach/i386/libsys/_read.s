.sect .text; .sect .rom; .sect .data; .sect .bss
.define __read
.sect .text
__read:
	mov	ax,3
	jmp	syscal
