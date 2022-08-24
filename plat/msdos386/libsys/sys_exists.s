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

! Say whether a file exists with the given name.

.define __sys_exists
__sys_exists:
	push esi
	mov esi, 4+4(esp)
	movzx edi, (transfer_buffer_ptr)
	mov edx, edi
	mov es, (pmode_ds)
	cld
1:
	lodsb
	stosb
	testb al, al
	jnz 1b
	mov eax, 0x4300
	mov ebx, 0x210000
	callf (interrupt_ptr)
	push ss
	pop es
	sbb eax, eax
	inc eax
	ret
