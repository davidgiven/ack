.define _dup
.define _dup2
.extern _dup
.text
_dup:
tst.b -40(sp)
link	a6,#-0
move.w 8(a6), d2
ext.l d2
move.l d2,-(sp)
jsr __Sdup
add.l #4,sp
unlk a6
rts
.extern _dup2
_dup2:
tst.b -40(sp)
link	a6,#-0
move.w 10(a6), d2
ext.l d2
move.w 8(a6), d1
ext.l d1
move.l d2,-(sp)
move.l d1,-(sp)
jsr __Sdup2
add.l #8,sp
unlk a6
rts
__Sdup2:		or.l	#64,4(sp)

__Sdup:			trap #0
.short	0x29
			bcc	1f
			jmp	cerror
1:
			rts
