#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __gettimeofday
__gettimeofday: SYSTEM(SYS_gettimeofday)
