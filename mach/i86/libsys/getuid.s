.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getuid
.extern _getuid, cerror
_getuid:	int 0x98
	jb 9f
	ret
9:
	jmp cerror
