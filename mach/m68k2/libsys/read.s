.define _read
.extern _read
.text
_read:		move.w #0x3,d0
		move.w 4(sp),a0
		move.l 6(sp),d1
		move.w 10(sp),a1
		jmp call
