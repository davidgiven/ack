.define _lock
.extern _lock
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_lock:		move.w #0x35,d0
		move.l 4(sp),a0
		jmp callc
