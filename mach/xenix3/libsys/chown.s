.sect .text; .sect .rom; .sect .data; .sect .bss
.define _chown
.sect .text
_chown:
	mov	ax,16
	jmp	sysal
