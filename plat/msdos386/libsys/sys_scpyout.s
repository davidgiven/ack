#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! .sys_scpyout: copy the ASCIIZ string starting from ds:eax to the
! transfer buffer.  The resulting ASCIIZ string, including the final null,
! should fit into the entire transfer buffer.  Return eax := offset of
! transfer buffer in real mode data segment.  Return CF = 1 if and only if
! the ASCIIZ string is too long to fit.  Preserve all other registers,
! except the flags.
!
! .sys_sncpyout: copy the ASCIIZ string starting from ds:eax to offset
! edx into the transfer buffer.  The resulting ASCIIZ string should occupy
! at most ecx bytes, including the final null.  Return eax : offset of
! string in real mode data segment.  Return CF = 1 if and only if the ASCIIZ
! string is too long to fit.  Preserve all other registers, except the
! flags.

.extern .sys_scpyout
.extern .sys_sncpyout
.sys_scpyout:
	push ecx
	push edx
	mov ecx, TRANSFER_BUFFER_SIZE
	xor edx, edx
	jmp .cont
.sys_sncpyout:
	push ecx
	push edx
.cont:
	push esi
	push edi
	mov esi, eax
	movzx edi, (transfer_buffer_ptr)
	add edi, edx
	mov edx, edi
	mov es, (pmode_ds)
	jcxz .error
.loop:
	lodsb
	stosb
	testb al, al
	loopnz .loop
	jz .ok
.error:
	stc
.ok:
	xchg edx, eax
	mov edx, ss
	mov es, edx
	pop edi
	pop esi
	pop edx
	pop ecx
	ret
