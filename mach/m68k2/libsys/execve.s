.define _execve
.extern _execve
.text
_execve:	move.w #0x3B,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp calle
