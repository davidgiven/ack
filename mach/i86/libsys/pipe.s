.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _pipe
.extern _pipe, cerror
_pipe:	int 0xaa
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
