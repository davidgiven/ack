.define _lseek
.extern _lseek
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_lseek:		move.l #0x13,d0
		jmp (call)
