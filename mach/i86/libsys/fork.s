.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _fork
.extern _fork, cerror
_fork:	int 0x82
	jmp 1f
	jae 2f
	jmp cerror
1:
	xor ax,ax
2:
	ret
