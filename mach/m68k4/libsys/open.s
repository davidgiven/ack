.define _open
.extern _open
.text
_open:		move.w #0x5,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp call
