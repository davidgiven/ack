#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _settimeofday
_settimeofday: SYSTEM(SYS_settimeofday)
