.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _fstat
.extern _fstat, cerror
_fstat:	int 0x9c
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
