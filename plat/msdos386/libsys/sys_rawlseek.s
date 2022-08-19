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

! Move the current file position for a file descriptor.

.define __sys_rawlseek
__sys_rawlseek:
	movb ah, 0x42
	mov ebx, esp
	mov edx, 8(bx)
	mov ecx, 12(bx)
	movb al, 16(bx)
	mov ebx, 4(bx)
	int 0x21
	jmp .sys_dxaxret
