.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __lseek
.extern __lseek, cerror
__lseek:	int 0x93
	jb 9f
	ret
9: jmp cerror
