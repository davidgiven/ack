#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __sigpause
__sigpause: SYSTEM(SYS_sigpause)
