.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _stat
.extern _stat, cerror
_stat:	int 0x92
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
