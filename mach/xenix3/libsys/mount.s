.sect .text; .sect .rom; .sect .data; .sect .bss
.define _mount
.sect .text
_mount:
	mov	ax,21
	jmp	sysal
