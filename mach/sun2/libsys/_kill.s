#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __kill
__kill: SYSTEM(SYS_kill)
