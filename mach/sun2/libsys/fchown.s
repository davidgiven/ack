#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _fchown
_fchown: SYSTEM(SYS_fchown)
