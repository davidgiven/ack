.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _read
.extern _read, cerror
_read:	int 0x83
	jb 9f
	ret
9: jmp cerror
