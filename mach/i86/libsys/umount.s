.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _umount
.extern _umount, cerror
_umount:	int 0x96
	jb 9f
	xor ax,ax
	ret
9:
	jmp cerror
