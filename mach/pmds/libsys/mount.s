.define _mount
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _mount
.sect .text
_mount:
tst.b -40(sp)
link	a6,#-0
move.w 16(a6), d2
ext.l d2
move.l d2,-(sp)
move.l 12(a6),-(sp)
move.l 8(a6),-(sp)
jsr __Smount
lea 12(sp),sp
unlk a6
rts
__Smount:		trap #0
.data2	0x15
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
