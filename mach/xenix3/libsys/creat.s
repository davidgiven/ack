.sect .text; .sect .rom; .sect .data; .sect .bss
.define _creat
.sect .text
_creat:
	mov	ax,8
	jmp	sysal
