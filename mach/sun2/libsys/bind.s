#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _bind
_bind: SYSTEM(SYS_bind)
