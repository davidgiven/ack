.define __exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __exit
__exit:		trap #0
.data2	0x1
