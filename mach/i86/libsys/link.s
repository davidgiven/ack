.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _link
.extern _link, cerror
_link:	int 0x89
	jb 9f
	ret
9: jmp cerror
