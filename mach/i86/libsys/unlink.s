.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _unlink
.extern _unlink, cerror
_unlink:	int 0x8a
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
