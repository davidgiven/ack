.sect .text; .sect .rom; .sect .data; .sect .bss
.define __pause
.sect .text
__pause:
	mov	ax,29
	jmp	syscal
