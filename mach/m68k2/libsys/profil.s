.define _profil
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _profil
.sect .text
_profil:	move.w #0x2C,d0
		move.l 4(sp),a0
		move.w 8(sp),d1
		ext.l  d1
		move.w 10(sp),a1
		move.w 12(sp),d2
		ext.l  d2
		trap #0
		rts
