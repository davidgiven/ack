.sect .text; .sect .rom; .sect .data; .sect .bss
.define _umask
.sect .text
_umask:
	mov	ax,60
	jmp	syscal
