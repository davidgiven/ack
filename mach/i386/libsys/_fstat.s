.sect .text; .sect .rom; .sect .data; .sect .bss
.define __fstat
.sect .text
__fstat:
	mov	ax,28
	jmp	sysx
