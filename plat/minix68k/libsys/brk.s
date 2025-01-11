.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _brk
.define _sbrk
_brk:
	jmp __brk
_sbrk:
	jmp __sbrk
