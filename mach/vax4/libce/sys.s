.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text

.define __write, __brk

__write:	jmp	_write+2

__brk:		jmp	_brk+2
