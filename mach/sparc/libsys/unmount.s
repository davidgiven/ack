#include "SYS.h"

.globl _unmount

SYS_call_1(umount)

_unmount:	
	b	_umount
