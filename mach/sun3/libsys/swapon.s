#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _swapon
_swapon: SYSTEM(SYS_swapon)
