.define __dup
.define __dup2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __dup
.sect .text
__dup:
tst.b -40(sp)
link	a6,#-0
move.w 8(a6), d2
ext.l d2
move.l d2,-(sp)
jsr __Sdup
add.l #4,sp
unlk a6
rts
.extern __dup2
__dup2:
tst.b -40(sp)
link	a6,#-0
move.w 10(a6), d2
ext.l d2
move.w 8(a6), d1
ext.l d1
move.l d2,-(sp)
move.l d1,-(sp)
jsr __Sdup2
add.l #8,sp
unlk a6
rts
__Sdup2:		or.l	#64,4(sp)

__Sdup:			trap #0
.data2	0x29
			bcc	1f
			jmp	cerror
1:
			rts
