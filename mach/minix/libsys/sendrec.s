.sect .text; .sect .rom; .sect .data
.define _send, _receive, _sendrec
.extern _send, _receive, _sendrec
! See ../h/com.h for C definitions
SEND = 1
RECEIVE = 2
BOTH = 3
SYSVEC = 32

!*========================================================================*
!                           send and receive                              *
!*========================================================================*
! send(), receive(), sendrec() all save bp, but destroy ax, bx, and cx.

.sect .text

_send:	mov cx,SEND		! send(dest, ptr)
	jmp L0

_receive:
	mov cx,RECEIVE		! receive(src, ptr)
	jmp L0

_sendrec:
	mov cx,BOTH		! sendrec(srcdest, ptr)
	jmp L0

  L0:	push bp			! save bp
	mov bp,sp		! can't index off sp
	mov ax,4(bp)		! ax = dest-src
	mov bx,6(bp)		! bx = message pointer
	int SYSVEC		! trap to the kernel
	pop bp			! restore bp
	ret			! return

