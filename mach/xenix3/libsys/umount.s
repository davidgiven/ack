.sect .text; .sect .rom; .sect .data; .sect .bss
.define _umount
.sect .text
_umount:
	mov	ax,22
	jmp	sysal
