.define __alarm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __alarm
.sect .text
__alarm:
tst.b -40(sp)
link	a6,#-0
move.w 8(a6),-(sp)
clr.w -(sp)
jsr __Salarm
add.l #4,sp
unlk a6
rts
__Salarm:		trap #0
.data2	0x1B
			rts
