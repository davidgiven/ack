.define __creat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __creat
.sect .text
__creat:
tst.b -40(sp)
link	a6,#-0
move.w 12(a6), d2
ext.l d2
move.l d2,-(sp)
move.l 8(a6),-(sp)
jsr __Screat
add.l #8,sp
unlk a6
rts
__Screat:		trap #0
.data2	0x8
			bcc	1f
			jmp	cerror
1:
			rts
