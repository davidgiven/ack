.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __kill
.extern __kill, cerror
__kill:	int 0xa5
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
