.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __getpid
.extern __getpid, cerror
__getpid:	int 0x94
	jb 9f
	ret
9:
	jmp cerror
