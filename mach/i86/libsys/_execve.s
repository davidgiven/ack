.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __execve
.extern __execve, cerror
__execve:
	int 0x8b
	jb 9f
	ret
9: jmp cerror
