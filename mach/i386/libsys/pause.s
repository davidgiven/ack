.sect .text; .sect .rom; .sect .data; .sect .bss
.define _pause
.sect .text
_pause:
	mov	ax,29
	jmp	syscal
