.define __getpid
.extern __getpid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__getpid:	move.l #0x14,d0
		trap #0
		rts
