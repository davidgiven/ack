.define _fstat
.extern _fstat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_fstat:		move.l	#0x1C,d0
		jmp (callc)
