.define _setgid
.extern _setgid
.text
_setgid:	move.w #0x2E,d0
		move.w 4(sp),a0
		jmp callc
