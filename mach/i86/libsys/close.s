.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _close
.extern _close, cerror
_close:	int 0x86
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
