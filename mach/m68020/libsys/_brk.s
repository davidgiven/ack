.define __brk
.define __sbrk
.extern __brk
.extern __sbrk
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__sbrk:		move.l (.limhp),d0
		add.l  d0,(4,sp)
		move.l #0x11,d0
		trap #0
		bcs lcerror
		move.l (.limhp),d0
		move.l d0,a0
		move.l (4,sp),(.limhp)
		rts
lcerror:	jmp (cerror)
__brk:		move.l #0x11,d0
		trap #0
		bcs lcerror
		move.l (4,sp),(.limhp)
		clr.l d0
		rts
