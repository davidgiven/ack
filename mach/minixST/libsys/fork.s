.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _fork
_fork:
	jmp __fork
