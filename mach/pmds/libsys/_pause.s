.define __pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __pause
__pause:		trap #0
.data2 29
		bcc 1f
		jmp cerror
1:
		rts
