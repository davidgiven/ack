.sect .text
.define _lstat
_lstat:
	mov eax, 190
	int 0x80
	jb .set_errno
	ret
