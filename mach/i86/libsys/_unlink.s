.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __unlink
.extern __unlink, cerror
__unlink:	int 0x8a
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
