.sect .text; .sect .rom; .sect .data; .sect .bss
.define __close
.sect .text
__close:
	mov	ax,6
	jmp	sysal
