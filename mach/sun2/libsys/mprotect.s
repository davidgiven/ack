#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mprotect
_mprotect: SYSTEM(SYS_mprotect)
