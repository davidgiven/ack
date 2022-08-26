#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! Rename a file.

.define _rename
_rename:
	push edi

	! Destination filename.

	mov eax, 4+8(esp)
#define DEST_NAME_OFFSET (TRANSFER_BUFFER_SIZE / 2)
	mov edx, DEST_NAME_OFFSET
	mov ecx, edx
	call .sys_sncpyout
	jc 9f
	xchg edi, eax
	
	! Source filename.

	mov eax, 4+4(esp)
	xor edx, edx
	call .sys_sncpyout
	jc 9f
	xchg edx, eax

	! Make the DOS call.

	movb ah, 0x56
	call .sys_dpmidos

	pop edi
	jmp .sys_zret

9:
	pop edi
	jmp .sys_toolongret
