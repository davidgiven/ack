.define _access
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _access
_access:		trap #0
.data2	0x21
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
