.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _exece
.extern _exece, cerror
_exece:	int 0xdb
	jb 9f
	ret
9: jmp cerror
