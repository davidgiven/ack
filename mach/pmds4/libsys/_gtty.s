.define __gtty
.extern __gtty
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__gtty:
tst.b -40(sp)
link	a6,#-0
move.l 12(a6),-(sp)
move.l #0x7408,-(sp)
move.l 8(a6),-(sp)
jsr __ioctl
add.l #12,sp
unlk a6
rts
