.define _fork
.extern _fork
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_fork:		move.l #0x2,d0
		trap #0
		bcc 1f		! 68020 way to do it
		jmp (cerror)
1:
		tst.b d1
		beq 2f
		clr.l d0
2:
		rts
