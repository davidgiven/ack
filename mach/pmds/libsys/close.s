.define _close
.extern _close
.text
_close:
tst.b -40(sp)
link	a6,#-0
move.w 8(a6), d2
ext.l d2
move.l d2,-(sp)
jsr __Sclose
add.l #4,sp
unlk a6
rts
__Sclose:		trap #0
.short	0x6
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
