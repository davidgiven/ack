.define _dup2
.extern _dup2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_dup2:
link	a6,#0
move.l (12,a6),-(sp)
move.l #0,-(sp)
move.l (8,a6),-(sp)
jsr (_fcntl)
add.l #12,sp
unlk a6
rts
