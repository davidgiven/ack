.sect .text; .sect .rom; .sect .data; .sect .bss
.define __creat
.sect .text
__creat:
	mov	ax,8
	jmp	sysal
