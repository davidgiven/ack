.define _getuid
.define _geteuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _getuid
.extern _geteuid
_getuid:		trap #0
.data2	0x18
			rts

_geteuid:		trap #0
.data2	0x18
			move.l	d1,d0
			rts
