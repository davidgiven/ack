.define _brk
.define _sbrk
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _brk
.extern _sbrk
.sect .text
_sbrk:		move.l .limhp,a0
		add.w  4(sp),a0
		move.w #0x11,d0
		trap #0
		bcs Icerror
		move.l .limhp,d0
		move.l d0,a0
		add.w  4(sp),a0
		move.l a0,.limhp
		rts
Icerror:	jmp cerror
_brk:		move.w #0x11,d0
		move.l 4(sp),a0
		trap #0
		bcs Icerror
		move.l 4(sp),.limhp
		clr.l d0
		rts
