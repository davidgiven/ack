.define __execve
.extern __execve
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__execve:	move.l #0x3B,d0
		jmp (calle)
