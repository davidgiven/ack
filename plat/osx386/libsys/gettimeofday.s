.sect .text
.define _gettimeofday
_gettimeofday:
	mov eax, 116
	int 0x80
	jb .set_errno
	ret
