.define _exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _exit
.sect .text
_exit:
tst.b -40(sp)
link	a6,#-0
jsr __cleanup
move.w 8(a6),-(sp)
jsr __exit
add.l #2,sp
unlk a6
rts
