.define _ioctl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _ioctl
_ioctl:		trap #0
.data2	0x36
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
