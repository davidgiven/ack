.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mount
.extern _mount, cerror
_mount:	int 0x95
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
