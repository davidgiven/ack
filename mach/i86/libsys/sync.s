.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sync
.extern _sync, cerror
_sync:	int 0xa4
	jb 9f
	ret
9:
	jmp cerror
