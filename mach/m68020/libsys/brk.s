.define _brk
.define _sbrk
.extern _brk
.extern _sbrk
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_sbrk:		move.l (.limhp),d0
		add.l  d0,(4,sp)
		move.l #0x11,d0
		trap #0
		bcs lcerror
		move.l (.limhp),d0
		move.l d0,a0
		move.l (4,sp),(.limhp)
		rts
lcerror:	jmp (cerror)
_brk:		move.l #0x11,d0
		trap #0
		bcs lcerror
		move.l (4,sp),(.limhp)
		clr.l d0
		rts
