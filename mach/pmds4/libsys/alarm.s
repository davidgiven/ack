.define _alarm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _alarm
.sect .text
_alarm:		trap #0
.data2	0x1B
			rts
