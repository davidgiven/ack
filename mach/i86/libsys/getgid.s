.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getgid
.extern _getgid, cerror
_getgid:	int 0xaf
	jb 9f
	ret
9:
	jmp cerror
