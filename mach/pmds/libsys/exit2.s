.define __exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __exit
.sect .text
__exit:
tst.b -40(sp)
link	a6,#-0
move.w 8(a6), d2
ext.l d2
move.l d2,-(sp)
jsr __Sexit
add.l #4,sp
unlk a6
rts
__Sexit:		trap #0
.data2			0x1
