.define _acct
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _acct
.sect .text
_acct:		move.w #0x33,d0
		move.l 4(sp),a0
		jmp callc
