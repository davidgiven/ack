.define __alarm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __alarm
.sect .text
__alarm:		trap #0
.data2	0x1B
			rts
