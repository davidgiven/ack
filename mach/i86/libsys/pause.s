.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _pause
.extern _pause, cerror
_pause:	int 0x9d
	jb 9f
	ret
9:
	jmp cerror
