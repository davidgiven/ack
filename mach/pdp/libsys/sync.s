#include "sys.h"
.globl	_sync

_sync:
	sys	sync
	rts	pc
