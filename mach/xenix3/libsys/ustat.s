.sect .text; .sect .rom; .sect .data; .sect .bss
.define _ustat
.sect .text
_ustat:
	mov	ax,569
	jmp	sysal
