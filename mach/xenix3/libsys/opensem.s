.sect .text; .sect .rom; .sect .data; .sect .bss
.define _opensem
.sect .text
_opensem:
	mov	ax,808
	jmp	sysal
