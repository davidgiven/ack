#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mremap
_mremap: SYSTEM(SYS_mremap)
