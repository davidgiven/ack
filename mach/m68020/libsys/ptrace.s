.define _ptrace
.extern _ptrace
.extern _errno
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_ptrace:	move.l #0x1A,d0
		clr.l  (_errno)
		jmp (call)
