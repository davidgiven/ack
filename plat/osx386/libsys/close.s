.sect .text
.define _close
_close:
	mov eax, 6
	int 0x80
	jb .set_errno
	ret
