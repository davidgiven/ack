.define _fork
.extern _fork
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_fork:		move.w #0x2,d0
		trap #0
		bra 1f
		bcc 2f
		jmp cerror
1:
		!move.l d0,p_uid
		clr.l d0
2:
		rts
