.define _umask
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _umask
.sect .text
_umask:		trap #0
.data2	0x3C
			rts
