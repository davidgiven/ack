.define call
.define callc
.define calle
.define cerror
.extern call
.extern callc
.extern calle
.extern cerror
.extern _errno
.sect .text
.sect .rom
.sect .data
.sect .bss
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
		move.l d0,_errno
		move.l #-1,d0
		rts
