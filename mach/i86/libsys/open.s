.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _open
.extern _open, cerror
_open:	int 0x85
	jb 9f
	ret
9: jmp cerror
