.define _lseek
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _lseek
.sect .text
_lseek:
tst.b -40(sp)
link	a6,#-0
move.w 14(a6), d2
ext.l d2
move.w 8(a6), d1
ext.l d1
move.l d2,-(sp)
move.l 10(a6),-(sp)
move.l d1,-(sp)
jsr __Slseek
lea 12(sp),sp
unlk a6
rts
__Slseek:		trap #0
.data2	0x13
			bcc	1f
			jmp	cerror
1:
			rts
