.define _fcntl
.extern _fcntl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_fcntl:		move.l #0x3E,d0
		jmp (callc)
