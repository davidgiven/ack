#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _flock
_flock: SYSTEM(SYS_flock)
