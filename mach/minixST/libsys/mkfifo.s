.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _mkfifo
_mkfifo:
	jmp __mkfifo
