.define _gtty
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _gtty
.sect .text
_gtty:
tst.b -40(sp)
link	a6,#-0
move.l 10(a6),-(sp)
move.w #29704,-(sp)
move.w 8(a6),-(sp)
jsr _ioctl
add.l #8,sp
unlk a6
rts
