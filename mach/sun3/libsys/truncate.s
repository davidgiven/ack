#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _truncate
_truncate: SYSTEM(SYS_truncate)
