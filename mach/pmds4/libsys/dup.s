.define _dup
.define _dup2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _dup
.sect .text
.extern _dup2
_dup2:		or.l	#64,4(sp)

_dup:			trap #0
.data2	0x29
			bcc	1f
			jmp	cerror
1:
			rts
