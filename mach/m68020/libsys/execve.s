.define _execve
.extern _execve
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_execve:	move.l #0x3B,d0
		jmp (calle)
