.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getpid
.extern _getpid, cerror
_getpid:	int 0x94
	jb 9f
	ret
9:
	jmp cerror
