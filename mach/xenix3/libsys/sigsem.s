.sect .text; .sect .rom; .sect .data; .sect .bss
.define _sigsem
.sect .text
_sigsem:
	mov	ax,1064
	jmp	sysal
