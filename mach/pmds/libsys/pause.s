.define _pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _pause
_pause:		trap #0
.data2 29
		bcc 1f
		jmp cerror
1:
		rts
