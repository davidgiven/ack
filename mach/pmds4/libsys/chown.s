.define _chown
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _chown
.sect .text
_chown:		trap #0
.data2	0x10
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
