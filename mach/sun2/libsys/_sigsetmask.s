#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __sigsetmask
__sigsetmask: SYSTEM(SYS_sigsetmask)
