.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _chmod
.extern _chmod, cerror
_chmod:	int 0x8f
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
