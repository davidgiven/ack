.define __exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern __exit
.sect .text
__exit:		move.w #0x1,d0
		move.w 4(sp),a0
		trap #0
