.define __getpid
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern __getpid
.sect .text
__getpid:	move.w #0x14,d0
		trap #0
		rts
