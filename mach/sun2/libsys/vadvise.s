#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _vadvise
_vadvise: SYSTEM(SYS_vadvise)
