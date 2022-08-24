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

! Create or truncate a file.

.define __sys_rawcreat
__sys_rawcreat:
	! Copy filename to transfer buffer.

	mov ebx, esp
	push esi
	push edi
	mov esi, 4(ebx)
    movzx edi, (transfer_buffer_ptr)
    mov es, (pmode_ds)
    cld
1:
	lodsb
	stosb
	testb al, al
	jnz 1b

	! Make the DOS call.

	movb ah, 0x3c
	movzx edx, (transfer_buffer_ptr)
	movb al, 8(ebx)
	mov ebx, 0x210000
	callf (interrupt_ptr)

	pop edi
	pop esi
	push ss
	pop es
	jmp .sys_axret

