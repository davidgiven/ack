.sect .text
.define _getpid
_getpid:
	mov eax, 20
	int 0x80
	jb .set_errno
	ret
