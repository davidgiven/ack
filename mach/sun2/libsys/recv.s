#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _recv
_recv: SYSTEM(SYS_recv)
