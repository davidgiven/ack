.define _alarm
.extern _alarm
.text
_alarm:
tst.b -40(sp)
link	a6,#-0
move.w 8(a6),-(sp)
clr.w -(sp)
jsr __Salarm
add.l #4,sp
unlk a6
rts
__Salarm:		trap #0
.short	0x1B
			rts
