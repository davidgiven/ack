.sect .text; .sect .rom; .sect .data; .sect .bss
.define _waitsem
.sect .text
_waitsem:
	mov	ax,1320
	jmp	sysal
