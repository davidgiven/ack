#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sigstack
_sigstack: SYSTEM(SYS_sigstack)
