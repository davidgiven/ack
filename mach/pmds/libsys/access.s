.define _access
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _access
.sect .text
_access:
tst.b -40(sp)
link	a6,#-0
move.w 12(a6), d2
ext.l d2
move.l d2,-(sp)
move.l 8(a6),-(sp)
jsr __Saccess
add.l #8,sp
unlk a6
rts
__Saccess:		trap #0
.data2	0x21
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
