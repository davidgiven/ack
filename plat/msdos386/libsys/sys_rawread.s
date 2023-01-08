#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! Read bytes from a file descriptor.  These routines do not do any
! translation between CRLF and LF line endings.
!
! Note that, for MS-DOS, a raw "write" request of zero bytes will truncate
! (or extend) the file to the current file position.

.define __sys_rawread
__sys_rawread:
file_handle = 1*4
write_buffer = 2*4
amount_to_read = 3*4

	mov eax, amount_to_read(esp)
	mov ecx, TRANSFER_BUFFER_SIZE
	cmp eax, ecx
	jge 2f
	mov ecx, eax
2:

	! Read from DOS into the transfer buffer.

	movb ah, 0x3f
	o16 mov dx, (transfer_buffer_ptr)
	o16 mov bx, file_handle(esp)
	call .sys_dpmidos
	jc 3f

	! Copy eax bytes out of the transfer buffer.

	movzx ecx, ax
	mov eax, write_buffer(esp)
	call .sys_cpyin

	xchg eax, ecx
	ret

3:
	! Process errors.

	push eax
	call __sys_seterrno
	pop ecx
	ret
