#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _fchmod
_fchmod: SYSTEM(SYS_fchmod)
