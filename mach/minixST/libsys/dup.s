.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _dup
_dup:
	jmp __dup
