.define _mount
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _mount
.sect .text
_mount:		trap #0
.data2	0x15
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
