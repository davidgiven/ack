#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _ftruncate
_ftruncate: SYSTEM(SYS_ftruncate)
