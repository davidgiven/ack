.sect .text
.define _lseek
_lseek:
	! ack passes 4-byte off_t, but system call takes 8-byte off_t
	mov eax, esp
	push 12(eax)		! whence
	push 0			! offset (high long)
	push 8(eax)		! offset (low long)
	push 4(eax)		! fd
	call 1f
	add esp, 16
	ret
1:
	mov eax, 199
	int 0x80
	jb .set_errno
	ret
