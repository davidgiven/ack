.define _fstat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _fstat
_fstat:			trap #0
.data2	0x1C
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
