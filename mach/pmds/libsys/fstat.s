.define _fstat
.extern _fstat
.text
_fstat:
tst.b -40(sp)
link	a6,#-0
move.w 8(a6), d2
ext.l d2
move.l 10(a6),-(sp)
move.l d2,-(sp)
jsr __Sfstat
add.l #8,sp
unlk a6
rts
__Sfstat:		trap #0
.short	0x1C
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
