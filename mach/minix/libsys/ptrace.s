.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _ptrace
_ptrace:
	jmp __ptrace
