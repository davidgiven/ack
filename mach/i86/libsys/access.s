.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _access
.extern _access, cerror
_access:	int 0xa1
	jb 9f
	xor ax,ax
	ret
9:
	jmp cerror
