.define _getpid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _getpid
_getpid:		trap #0
.data2	0x14
			rts
