.define _chown
.extern _chown
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_chown:		move.l #0x10,d0
		jmp (callc)
