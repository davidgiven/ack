.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __fork
.extern __fork, cerror
__fork:	int 0x82
	jmp 1f
	jae 2f
	jmp cerror
1:
	xor ax,ax
2:
	ret
