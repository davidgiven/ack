.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __open
.extern __open, cerror
__open:	int 0x85
	jb 9f
	ret
9: jmp cerror
