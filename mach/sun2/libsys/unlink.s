#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _unlink
_unlink: SYSTEM(SYS_unlink)
