.define _chdir
.extern _chdir
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_chdir:		move.l #0xC,d0
		jmp (callc)
