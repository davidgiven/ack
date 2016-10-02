.sect .text
.define _read
_read:
	mov eax, 3
	int 0x80
	jb .set_errno
	ret
