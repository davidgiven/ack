.define _acct
.extern _acct
.text
_acct:		move.w #0x33,d0
		move.l 4(sp),a0
		jmp callc
