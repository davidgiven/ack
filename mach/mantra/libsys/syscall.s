.define _syscall
.extern _syscall
.extern	cerror
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_syscall:
		move.l 4(sp),d0
		move.l 8(sp),a0
		move.l 12(sp),d1
		move.l 16(sp),a1
		movem.l d2/d3/a2,sv
		move.l	20(sp),d2
		move.l	24(sp),a2
		move.l	28(sp),d3
		trap	#0
		movem.l	sv,d2/d3/a2
		bcs	1f
		rts
	1:
		jmp	cerror

.sect .bss
sv: .space 12
