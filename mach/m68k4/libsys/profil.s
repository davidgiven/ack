.define _profil
.extern _profil
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_profil:	move.w #0x2C,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		move.l 16(sp),d2
		trap #0
		rts
