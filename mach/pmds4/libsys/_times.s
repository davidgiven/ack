.define __times
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __times
__times:
	trap #0
.data2	0x2B
	rts
