.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _getcwd
_getcwd:
	jmp __getcwd
