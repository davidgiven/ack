#include "sys.h"
.globl _nostk

_nostk:
	sys	local; 0f
	rts	pc
.data
0:
	sys	nostk
