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
	mov edx, 8(ebx)
	mov ecx, edx
	shr ecx, 16
	movb al, 12(ebx)
	mov ebx, 4(ebx)
	int 0x21
	jmp .sys_dxaxret
