#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _gettimeofday
_gettimeofday: SYSTEM(SYS_gettimeofday)
