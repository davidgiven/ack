.define _lock
.extern _lock
.text
_lock:		move.w #0x35,d0
		move.w 4(sp),a0
		jmp callc
