.sect .text; .sect .rom; .sect .data; .sect .bss
.define _creatsem
.sect .text
_creatsem:
	mov	ax,552
	jmp	sysal
