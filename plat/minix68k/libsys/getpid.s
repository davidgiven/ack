.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _getpid
_getpid:
	jmp __getpid
