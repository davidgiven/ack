#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _kill
_kill: SYSTEM(SYS_kill)
