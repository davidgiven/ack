.sect .text
.define _open
_open:
	mov eax, 5
	int 0x80
	jb .set_errno
	ret
