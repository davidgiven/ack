.define __dup
.define __dup2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__dup2:		or.l	#64,4(sp)

__dup:			trap #0
.data2	0x29
			bcc	1f
			jmp	cerror
1:
			rts
