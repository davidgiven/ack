.define _exit
.extern _exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_exit:
tst.b -40(sp)
link	a6,#-0
jsr __cleanup
move.l 8(a6),-(sp)
jsr __exit
add.l #4,sp
unlk a6
rts
