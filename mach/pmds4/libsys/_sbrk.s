.define __sbrk
.define __brk
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__sbrk:		tst.b	-8(sp)
			move.l	4(sp),d0
			beq	1f
			add.l	.limhp,d0
			move.l	d0,-(sp)
			clr.l	-(sp)
			trap #0
.data2	0x11
			add.l	#8,sp
			bcc	1f
			jmp	cerror
1:
			move.l	.limhp,d0
			move.l	4(sp),d1
			add.l	d1,.limhp
			rts
__brk:			trap #0
.data2	0x11
			bcc	1f
			jmp	cerror
1:
			move.l	4(sp),.limhp
			clr.l	d0
			rts
