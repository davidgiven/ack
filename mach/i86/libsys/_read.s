.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __read
.extern __read, cerror
__read:	int 0x83
	jb 9f
	ret
9: jmp cerror
