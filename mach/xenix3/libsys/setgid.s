.sect .text; .sect .rom; .sect .data; .sect .bss
.define _setgid
.sect .text
_setgid:
	mov	ax,46
	jmp	sysal
