.sect .text; .sect .rom; .sect .data; .sect .bss
.define __lseek
.sect .text
__lseek:
	mov	ax,19
	jmp	syscal
