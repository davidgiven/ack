.define _close
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _close
_close:		trap #0
.data2	0x6
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
