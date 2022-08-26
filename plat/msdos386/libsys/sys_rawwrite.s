#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! Write bytes to/to a file descriptor.  These routines do not do any
! translation between CRLF and LF line endings.
!
! Note that, for MS-DOS, a raw "write" request of zero bytes will truncate
! (or extend) the file to the current file position.

.define __sys_rawwrite
__sys_rawwrite:
file_handle = 1*4
read_buffer = 2*4
amount_to_write = 3*4

    mov eax, amount_to_write(esp)
    mov ecx, TRANSFER_BUFFER_SIZE
    cmp eax, ecx
    jge 2f
    mov ecx, eax
2:

    ! Copy ecx bytes into the transfer buffer.

    mov eax, read_buffer(esp)
    call .sys_cpyout

    ! Write from the transfer buffer to DOS.

    xchg edx, eax
    movb ah, 0x40
    o16 mov bx, file_handle(esp)
    call .sys_dpmidos
    movzx eax, ax
    jnc exit

    push eax
    call __sys_seterrno
    pop ecx
exit:
    ret

! vim: sw=4 ts=4 et

