.sect .text; .sect .rom; .sect .data; .sect .bss
.define _close
.sect .text
_close:
	mov	ax,6
	jmp	sysal
