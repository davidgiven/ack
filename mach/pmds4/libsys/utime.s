.define _utime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _utime
_utime:			trap #0
.data2	0x1E
			bcc	1f
			jmp	cerror
1:
			rts
