.define _utime
.extern _utime
.text
_utime:		move.w #0x1E,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp callc
