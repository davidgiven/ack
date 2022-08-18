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
	mov ebx, esp
	mov eax, 0x4406
	mov ebx, 4(ebx)
	int 0x21
	jnc ok
	movb al, 0
ok:
	o16 cbw
	cwde
	ret
