.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _execve
.extern _execve, cerror
_execve:
	int 0x8b
	jb 9f
	ret
9: jmp cerror
