.define _sbrk
.define _brk
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _sbrk
.extern _brk
_sbrk:		tst.b	-8(sp)
			move.l	4(sp),d0
			beq	1f
			add.l	.reghp,d0
			move.l	d0,-(sp)
			clr.l	-(sp)
			trap #0
.data2	0x11
			add.l	#8,sp
			bcc	1f
			jmp	cerror
1:
			move.l	.reghp,d0
			move.l	4(sp),d1
			add.l	d1,.reghp
			rts
_brk:			trap #0
.data2	0x11
			bcc	1f
			jmp	cerror
1:
			move.l	4(sp),.reghp
			clr.l	d0
			rts
