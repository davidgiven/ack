.define _ptrace
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _ptrace
_ptrace:		trap #0
.data2	0x1A
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
