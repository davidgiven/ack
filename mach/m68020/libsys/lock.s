.define _lock
.extern _lock
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_lock:		move.l #0x35,d0
		jmp (callc)
