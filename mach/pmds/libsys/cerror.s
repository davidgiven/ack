.define cerror
.define _errno
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern cerror
cerror:			move.l d0,_errno
			move.l #-1,d0
			rts
.sect .bss
_errno:
			.space 4
.sect .text
