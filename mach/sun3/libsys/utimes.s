#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _utimes
_utimes: SYSTEM(SYS_utimes)
