.sect .text; .sect .rom; .sect .data; .sect .bss
.define _lseek
.sect .text
_lseek:
	mov	ax,19
	jmp	syscal
