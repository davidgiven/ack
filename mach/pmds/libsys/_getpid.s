.define __getpid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __getpid
__getpid:		trap #0
.data2	0x14
			rts
