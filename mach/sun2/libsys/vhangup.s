#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _vhangup
_vhangup: SYSTEM(SYS_vhangup)
