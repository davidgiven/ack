.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define ___statcvt
___statcvt:
move.l (4,sp),a0
lea (60,a0),a0
lea (4,a0),a1
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.w -(a0),-(a1)
clr.w -(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.l -(a0),-(a1)
move.w -(a0),-(a1)
clr.w -(a1)
move.w -(a0),-(a1)
clr.l d0
rts
