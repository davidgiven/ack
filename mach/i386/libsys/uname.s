.sect .text; .sect .rom; .sect .data; .sect .bss
.define _uname
.sect .text
_uname:
	mov	ax,57
	jmp	syscal
