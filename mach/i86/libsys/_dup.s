.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text

.define __dup
.extern __dup, cerror
__dup:	int 0xc9
	jb 9f
	ret
9: jmp cerror
