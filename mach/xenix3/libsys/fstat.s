.sect .text; .sect .rom; .sect .data; .sect .bss
.define _fstat
.sect .text
_fstat:
	mov	ax,28
	jmp	sysal
