.define __exit
.extern __exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__exit:		move.w #0x1,d0
		move.l 4(sp),a0
		trap #0
