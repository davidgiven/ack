#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _killpg
_killpg: SYSTEM(SYS_killpg)
