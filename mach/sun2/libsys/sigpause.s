#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sigpause
_sigpause: SYSTEM(SYS_sigpause)
