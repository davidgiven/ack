.define _umask
.extern _umask
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_umask:		move.l #0x3C,d0
		jmp (call)
