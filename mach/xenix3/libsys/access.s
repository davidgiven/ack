.sect .text; .sect .rom; .sect .data; .sect .bss
.define _access
.sect .text
_access:
	mov	ax,33
	jmp	sysal
