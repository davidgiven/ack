#include "sys.h"
.define	_sync

_sync:
	sys	sync
	rts	pc
