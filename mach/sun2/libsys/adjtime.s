#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _adjtime
_adjtime: SYSTEM(SYS_adjtime)
