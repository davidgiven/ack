.sect .text; .sect .rom; .sect .data; .sect .bss
.define _sbrk
.sect .text
_sbrk:
	jmp	__sbrk
