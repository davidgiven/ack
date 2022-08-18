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

! Write bytes to/to a file descriptor.  These routines do not do any
! translation between CRLF and LF line endings.
!
! Note that, for MS-DOS, a raw "write" request of zero bytes will truncate
! (or extend) the file to the current file position.

.define __sys_rawwrite
__sys_rawwrite:
    enter 0, 0
    push esi
    push edi
file_handle = 2*4
read_buffer = 3*4
amount_to_write = 4*4

    mov eax, amount_to_write(ebp)
    mov ecx, 32*1024        ! size of transfer buffer
    cmp eax, ecx
    jge 2f
    mov ecx, eax
2:

    ! Copy ecx bytes into the transfer buffer.

    push ecx
    mov esi, read_buffer(ebp)
    movzx edi, (transfer_buffer_ptr)
    mov es, (pmode_ds)
    cld
    rep movsb
    pop ecx

    ! Write from the transfer buffer to DOS.

    movb ah, 0x40
    o16 mov dx, (transfer_buffer_ptr)
    o16 mov bx, file_handle(ebp)
    or ebx, 0x210000
    callf (interrupt_ptr)
    jnc exit

    push eax
    call __sys_seterrno
    pop ecx
exit:
    pop edi
    pop esi
    leave
    ret

! vim: sw=4 ts=4 et

