! The system call checks the timeval pointer but doesn't store the
! time there.  If the pointer wasn't NULL, then the system call
! returns the time in a pair of registers.

.sect .text
.define _gettimeofday
_gettimeofday:
	mov eax, 116
	int 0x80
	jb .set_errno
	mov ebx, 4(esp)		! timeval pointer
	test ebx, ebx
	je 1f
	mov 0(ebx), eax		! seconds
	mov 4(ebx), edx		! microseconds
1:
	mov eax, 0		! return 0
	ret
