.define __lseek
.extern __lseek
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__lseek:		move.l #0x13,d0
		jmp (call)
