.define __alarm
.extern __alarm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__alarm:
		move.l 4(sp),a0
		move.l #0x1B,d0
		trap #0
		rts
