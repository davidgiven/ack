.define _getgid
.define _getegid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _getgid
.extern _getegid
_getgid:		trap #0
.data2	0x2F
			rts

_getegid:		trap #0
.data2	0x2F
			move.l	d1,d0
			rts
