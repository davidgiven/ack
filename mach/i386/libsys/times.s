.sect .text; .sect .rom; .sect .data; .sect .bss
.define _times
.sect .text
_times:
	mov	ax,43
	jmp	syscal
