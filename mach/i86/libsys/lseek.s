.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _lseek
.extern _lseek, cerror
_lseek:	int 0x93
	jb 9f
	ret
9: jmp cerror
