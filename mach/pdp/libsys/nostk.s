#include "sys.h"
.define _nostk

_nostk:
	sys	local; .data2 0f
	rts	pc
.sect .data
0:
	sys	nostk
