! OS X, unlike FreeBSD, requires us to provide our own signal
! trampoline.  We must change the new action from a struct sigaction
! to a bigger struct that includes the trampoline.

.sect .text
.define _sigaction
_sigaction:
	mov eax, esp
	mov ebx, 8(esp)		! ebx = ptr to new action
	cmp ebx, 0
	je 1f
	! push bigger struct
	push 8(ebx)		! sa_flags
	push 4(ebx)		! sa_mask
	push trampoline		! sa_tramp
	push 0(ebx)		! sa_handler
	mov ebx, esp
	jmp 2f
1:
	sub esp, 16
2:
	push 12(eax)		! ptr to old action
	push ebx		! ptr to bigger struct
	push 4(eax)		! sig
	call 3f
	add esp, 28
	ret
3:
	mov eax, 46
	int 0x80
	jb .set_errno
	ret

trampoline:
	! 4(esp) = handler
	! 8(esp) = info style
	! 12(esp) = sig
	! 16(esp) = info
	! 20(esp) = context

	! Call handler(sig, info, context)
	mov eax, esp
	push 20(eax)
	push 16(eax)
	push 12(eax)
	call 4(eax)
	add esp, 12

	! Return from trampoline.
	mov eax, esp
	push 8(eax)		! info style
	push 20(eax)		! context
	sub esp, 4
	mov eax, 184		! sigreturn
	int 0x80

	! Only if sigreturn() fails:
	mov eax, 1		! exit
	int 0x80
