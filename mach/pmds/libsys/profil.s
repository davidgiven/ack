.define _profil
.extern _profil
.text
_profil:
tst.b -40(sp)
link	a6,#-0
move.w 16(a6), d2
ext.l d2
move.w 14(a6), d1
ext.l d1
move.w 12(a6), d0
ext.l d0
move.l d2,-(sp)
move.l d1,-(sp)
move.l d0,-(sp)
move.l 8(a6),-(sp)
jsr __Sprofil
lea 16(sp),sp
unlk a6
rts
__Sprofil:		trap #0
.short	0x2C
			rts
