.define _chown
.extern _chown
.text
_chown:		move.w #0x10,d0
		move.l 4(sp),a0
		move.w 8(sp),d1
		ext.l  d1
		move.w 10(sp),a1
		jmp callc
