#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _rename
_rename: SYSTEM(SYS_rename)
