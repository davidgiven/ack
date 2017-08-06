.sect .text
.define _mmap
_mmap:
	! ack passes 4-byte off_t, but system call takes 8-byte off_t
	mov eax, esp
	push 0			! offset (high long)
	push 24(eax)		! offset (low long)
	push 20(eax)		! fd
	push 16(eax)		! flags
	push 12(eax)		! protection
	push 8(eax)		! length
	push 4(eax)		! address
	call 1f
	add esp, 28
	ret
1:
	mov eax, 197
	int 0x80
	jb .set_errno
	ret
