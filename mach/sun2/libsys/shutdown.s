#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _shutdown
_shutdown: SYSTEM(SYS_shutdown)
