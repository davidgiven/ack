.define call
.define callc
.define calle
.define cerror
.define _errno
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern call
.extern callc
.extern calle
.extern cerror
.extern _errno
.sect .text
call:		trap #0
		bcs cerror
		rts
callc:
		trap #0
		bcs cerror
		clr.l d0
		rts
calle:
		trap #0
cerror:
		move.w d0,_errno
		move.l #-1,d0
		rts
.sect .bss
_errno:		.space 4
.sect .text
