.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _kill
.extern _kill, cerror
_kill:	int 0xa5
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
