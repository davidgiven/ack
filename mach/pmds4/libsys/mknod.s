.define _mknod
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _mknod
.sect .text
_mknod:		trap #0
.data2	0xE
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
