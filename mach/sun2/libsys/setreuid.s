#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setreuid
_setreuid: SYSTEM(SYS_setreuid)
