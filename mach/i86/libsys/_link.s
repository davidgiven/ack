.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __link
.extern __link, cerror
__link:	int 0x89
	jb 9f
	ret
9: jmp cerror
