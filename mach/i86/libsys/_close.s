.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __close
.extern __close, cerror
__close:	int 0x86
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
