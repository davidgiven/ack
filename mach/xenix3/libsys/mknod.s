.sect .text; .sect .rom; .sect .data; .sect .bss
.define _mknod
.sect .text
_mknod:
	mov	ax,14
	jmp	sysal
