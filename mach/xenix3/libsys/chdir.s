.sect .text; .sect .rom; .sect .data; .sect .bss
.define _chdir
.sect .text
_chdir:
	mov	ax,12
	jmp	sysal
