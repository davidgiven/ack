.sect .text; .sect .rom; .sect .data; .sect .bss
.define _stat
.sect .text
_stat:
	mov	ax,18
	jmp	sysal
