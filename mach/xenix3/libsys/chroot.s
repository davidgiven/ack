.sect .text; .sect .rom; .sect .data; .sect .bss
.define _chroot
.sect .text
_chroot:
	mov	ax,61
	jmp	sysal
