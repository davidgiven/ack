.define _stime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _stime
.sect .text
_stime:		trap #0
.data2	0x19
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
