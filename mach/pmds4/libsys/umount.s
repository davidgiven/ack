.define _umount
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _umount
_umount:		trap #0
.data2	0x16
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
