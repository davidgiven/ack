.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define __write, __brk

__write:	jmp	(_write)

__brk:		jmp	(_brk)
