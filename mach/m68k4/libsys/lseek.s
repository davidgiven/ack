.define _lseek
.extern _lseek
.text
_lseek:		move.w #0x13,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp call
