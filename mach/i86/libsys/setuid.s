.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setuid
.extern _setuid, cerror
_setuid:	int 0x97
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
