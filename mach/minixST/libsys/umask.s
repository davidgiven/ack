.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _umask
_umask:
	jmp __umask
