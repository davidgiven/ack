.define __exit
.extern __exit
.text
__exit:		move.w #0x1,d0
		move.w 4(sp),a0
		trap #0
