.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sbrk
_sbrk:
	jmp __sbrk
