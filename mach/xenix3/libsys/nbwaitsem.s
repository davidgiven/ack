.sect .text; .sect .rom; .sect .data; .sect .bss
.define _nbwaitsem
.sect .text
_nbwaitsem:
	mov	ax,1576
	jmp	sysal
