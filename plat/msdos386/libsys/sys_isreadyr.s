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

! Say whether a file descriptor is ready for input, i.e. reading from the
! fd will immediately return something.

.define __sys_isreadyr
__sys_isreadyr:
	mov bx, sp
	mov ax, 0x4406
	mov bx, 2(bx)
	int 0x21
	jnc ok
	movb al, 0
ok:
	cbw
	ret
