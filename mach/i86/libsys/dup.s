.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text

.define _dup
.extern _dup, cerror
_dup:	int 0xc9
	jb 9f
	ret
9: jmp cerror
