#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

.define _brk
_brk:
	mov ebx, esp
	mov eax, 1*4(ebx)
	callf (realloc_ptr)
	xor eax, eax
	ret


