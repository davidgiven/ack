.define __exit
.extern __exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__exit:		move.l #0x1,d0
		trap #0
		stop #0			! was there in 68020 routine
