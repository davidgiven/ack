#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _access
_access: SYSTEM(SYS_access)
