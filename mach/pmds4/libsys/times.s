.define _times
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _times
_times:
	trap #0
.data2	0x2B
	rts
