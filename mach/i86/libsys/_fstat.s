.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __fstat
.extern __fstat, cerror
__fstat:	int 0x9c
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
