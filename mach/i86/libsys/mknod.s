.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mknod
.extern _mknod, cerror
_mknod:	int 0x8e
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
