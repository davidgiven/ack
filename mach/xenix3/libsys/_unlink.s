.sect .text; .sect .rom; .sect .data; .sect .bss
.define __unlink
.sect .text
__unlink:
	mov	ax,10
	jmp	sysal
