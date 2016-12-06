.sect .text
.define _write
_write:
	mov eax, 4
	int 0x80
	jb .set_errno
	ret
