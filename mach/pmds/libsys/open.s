.define _open
.extern _open
.text
_open:
tst.b -40(sp)
link	a6,#-0
move.w 12(a6), d2
ext.l d2
move.l d2,-(sp)
move.l 8(a6),-(sp)
jsr __Sopen
add.l #8,sp
unlk a6
rts
__Sopen:		trap #0
.short	0x5
			bcc	1f
			jmp	cerror
1:
			rts
