.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _write
.extern _write, cerror
_write:	int 0x84
	jb 9f
	ret
9: jmp cerror
