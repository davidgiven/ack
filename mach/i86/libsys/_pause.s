.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __pause
.extern __pause, cerror
__pause:	int 0x9d
	jb 9f
	ret
9:
	jmp cerror
