.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __pipe
.extern __pipe, cerror
__pipe:	int 0xaa
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
