#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sigsetmask
_sigsetmask: SYSTEM(SYS_sigsetmask)
