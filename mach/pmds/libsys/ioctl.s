.define _ioctl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _ioctl
.sect .text
_ioctl:
tst.b -40(sp)
link	a6,#-0
move.w 10(a6), d2
ext.l d2
move.w 8(a6), d1
ext.l d1
move.l 12(a6),-(sp)
move.l d2,-(sp)
move.l d1,-(sp)
jsr __Sioctl
lea 12(sp),sp
unlk a6
rts
__Sioctl:		trap #0
.data2	0x36
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
