.define _chown
.extern _chown
.text
_chown:
tst.b -40(sp)
link	a6,#-0
move.w 14(a6), d2
ext.l d2
move.w 12(a6), d1
ext.l d1
move.l d2,-(sp)
move.l d1,-(sp)
move.l 8(a6),-(sp)
jsr __Schown
lea 12(sp),sp
unlk a6
rts
__Schown:		trap #0
.short	0x10
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
