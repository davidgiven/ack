.define _stat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _stat
_stat:			trap #0
.data2	0x12
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
