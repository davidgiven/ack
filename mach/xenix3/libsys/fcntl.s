.sect .text; .sect .rom; .sect .data; .sect .bss
.define _fcntl
.sect .text
_fcntl:
	mov	ax,62
	jmp	sysal
