.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __write
.extern __write, cerror
__write:	int 0x84
	jb 9f
	ret
9: jmp cerror
