.define _dup2
.extern _dup2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_dup2:		move.l #0x29,d0
		add.l #0x40,(4,sp)	! ???
		jmp (call)
