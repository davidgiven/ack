.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _chdir
.extern _chdir, cerror
_chdir:	int 0x8c
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
