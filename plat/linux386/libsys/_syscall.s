#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

EINVAL = 22
	
! Perform a Linux system call.

.define __syscall

__syscall:
	mov eax, 4(esp)
	mov ebx, 8(esp)
	mov ecx, 12(esp)
	mov edx, 16(esp)

	int 0x80
	or eax, eax
	jl 1f
	ret
1:
	neg eax
	! It just so happens that errnos 1-34 are the same in Linux as in the ACK.
	cmp eax, 1
	jb 2f
	cmp eax, 34
	ja 2f
	mov (_errno), eax
3:
	mov eax, -1
	ret
2:
	! All other errnos become EINVAL for now. FIXME dtrg.
	mov (_errno), EINVAL
	jmp 3b
