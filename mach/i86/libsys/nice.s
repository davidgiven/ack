.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _nice
.extern _nice, cerror
_nice:	int 0xa2
	jb 9f
	ret
9:
	jmp cerror
