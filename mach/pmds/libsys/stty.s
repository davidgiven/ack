.define _stty
.extern _stty
.text
_stty:
tst.b -40(sp)
link	a6,#-0
move.l 10(a6),-(sp)
move.w #29705,-(sp)
move.w 8(a6),-(sp)
jsr _ioctl
add.l #8,sp
unlk a6
rts
