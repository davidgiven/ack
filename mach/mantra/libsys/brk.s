.define _brk
.define _sbrk
.extern _brk
.extern _sbrk
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_sbrk:		move.l .reghp,a0
		add.l  4(sp),a0
		move.l #0x11,d0
		trap #0
		bcs lcerror
		move.l .reghp,d0
		move.l d0,a0
		add.l  4(sp),a0
		move.l a0,.reghp
		rts
lcerror:	jmp cerror
_brk:		move.l #0x11,d0
		move.l 4(sp),a0
		trap #0
		bcs lcerror
		move.l 4(sp),.reghp
		clr.l d0
		rts
