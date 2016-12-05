.sect .text
.define _stat
_stat:
	mov eax, 188
	int 0x80
	jb .set_errno
	ret
