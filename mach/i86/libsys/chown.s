.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _chown
.extern _chown, cerror
_chown: int 0x90
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
