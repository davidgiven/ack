.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _getppid
_getppid:
	jmp __getppid
