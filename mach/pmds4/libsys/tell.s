.define _tell
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.extern _tell
_tell:		
			pea	1
			clr.l	-(sp)
			move.l	12(sp),-(sp)
			jsr	_lseek
			add	#12,sp
			rts
