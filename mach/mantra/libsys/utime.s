.define _utime
.extern _utime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_utime:		move.l #0x1E,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp callc
