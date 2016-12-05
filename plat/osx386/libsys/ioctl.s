.sect .text
.define _ioctl
_ioctl:
	mov eax, 54
	int 0x80
	jb .set_errno
	ret
